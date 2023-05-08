// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/SEdNode_Dialogue_Basic.h"
#include "DialogueEditorHeaders.h"
#include "DialogueEditor.h"
#include "DialogueAssetEditor/Colors_Dialogue.h"
#include "SLevelOfDetailBranchNode.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SCommentBubble.h"
#include "SlateOptMacros.h"
#include "SGraphPin.h"
#include "GraphEditorSettings.h"
#include "DialogueAssetEditor/DialogueDragConnection.h"
#include "DialogueSession.h"
#include "Styling/AppStyle.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Components/RichTextBlockDecorator.h"
#include "Styling/SlateStyle.h"
#include "Settings_DialogueEditor.h"
#include "Components/RichTextBlock.h"
#include "RenderingThread.h"
#include "SRichTextBlockDecorator.h"

template< class ObjectType >
struct FDialogueDeferredDeletor : public FDeferredCleanupInterface
{
public:
	FDialogueDeferredDeletor(ObjectType* InInnerObjectToDelete)
		: InnerObjectToDelete(InInnerObjectToDelete)
	{
	}

	virtual ~FDialogueDeferredDeletor()
	{
		delete InnerObjectToDelete;
	}

private:
	ObjectType* InnerObjectToDelete;
};

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdNode_Dialogue_Basic::Construct(const FArguments& InArgs, UEdGraphNode_Dialogue_Basic* InNode)
{
	EdNode = InNode;
	GraphNode = InNode;
	InNode->SEdNode = this;
	InNode->SEdNode_Basic = this;
	EdNode_Basic = InNode;
	RichTextBlock = NewObject<URichTextBlock>(EdNode_Basic, FName(TEXT("RichTextBlock")));

	UpdateGraphNode();
	UpdateRichTextStyle();
}

FText SEdNode_Dialogue_Basic::GetDialogueString() const
{
	FText ReturnText = FText::GetEmpty();

	if (EdNode_Basic != nullptr)
	{
		ReturnText = FText::FromString( EdNode_Basic->GetDialogueString());
	}

	return ReturnText;
}

FText SEdNode_Dialogue_Basic::GetOriginalDialogueString() const
{
	FText ReturnText = FText::GetEmpty();

	if (EdNode_Basic != nullptr)
	{
		ReturnText = FText::FromString(EdNode_Basic->GetDialogueString_Original());
	}

	return ReturnText;
}

EVisibility SEdNode_Dialogue_Basic::CanVisibleDialogue() const
{
	return EVisibility::Visible;
}

EVisibility SEdNode_Dialogue_Basic::CanVisibleOriginalDialogue() const
{
	TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();

	if (MyOwnerPanel.IsValid() && EdNode_Basic != nullptr)
	{
		return MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail ? EVisibility::Visible : EVisibility::Collapsed;
	}
	else
	{
		return EVisibility::Collapsed;
	}
}

EVisibility SEdNode_Dialogue_Basic::CanVisibleImportBTN() const
{
	TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();

	if (MyOwnerPanel.IsValid() && EdNode_Basic != nullptr && MyOwnerPanel->SelectionManager.IsNodeSelected(EdNode_Basic) )
	{
		return MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail ? EVisibility::Visible : EVisibility::Collapsed;
	}
	else
	{
		return EVisibility::Collapsed;
	}
}

FReply SEdNode_Dialogue_Basic::ImportDialogue()
{
	LOG_INFO(TEXT("Try Import Dialogue."));
	if (EdNode_Basic != nullptr) {
			EdNode_Basic->TryImportDialogueString();
	}

	return FReply::Handled();
}



//////////////////////////// Rich Text Block /////////////////////////////
template< class ObjectType >
FORCEINLINE TSharedPtr< ObjectType > MakeDialogueShareableDeferredCleanup(ObjectType* InObject)
{
	return MakeShareable(InObject, [](ObjectType* ObjectToDelete) { BeginCleanup(new FDialogueDeferredDeletor<ObjectType>(ObjectToDelete)); });
}

void SEdNode_Dialogue_Basic::MakeStyleInstance()
{
	StyleInstance.Reset();
	StyleInstance = MakeDialogueShareableDeferredCleanup(new FSlateStyleSet(TEXT("RichTextStyle")));
	
	UDataTable* DialogueTextStyleSet = EdNode_Basic->GetDialogueTextStyleSet();


	if (DialogueTextStyleSet && DialogueTextStyleSet->GetRowStruct()->IsChildOf(FRichTextStyleRow::StaticStruct()))
	{
		for (const auto& Entry : DialogueTextStyleSet->GetRowMap())
		{
			FName SubStyleName = Entry.Key;
			FRichTextStyleRow* RichTextStyle = (FRichTextStyleRow*)Entry.Value;

			if (SubStyleName == FName(TEXT("Default")))
			{
				DialogueDefaultTextStyle = RichTextStyle->TextStyle;
			}

			StyleInstance->Set(SubStyleName, RichTextStyle->TextStyle);
		}
	}
}

void SEdNode_Dialogue_Basic::MakeDecoInstance(TArray<TSharedRef<ITextDecorator>>& OutDecorators)
{
	ERichTextBlockType TextBlockType = EdNode_Basic->GetTextBlockType();
	InstanceSlateDecorators.Empty();
	InstanceUMGDecorators.Empty();

	switch (TextBlockType)
	{
		case ERichTextBlockType::SLATE:
			MakeSlateDecoInstance(OutDecorators);
		break;

		case ERichTextBlockType::UMG:
			MakeUMGDecoInstance(OutDecorators);
		break;
	}
}

void SEdNode_Dialogue_Basic::MakeUMGDecoInstance(TArray<TSharedRef<class ITextDecorator>>& OutDecorators)
{
	TArray<TSubclassOf<URichTextBlockDecorator>> DecoratorUMGClasses;

	for (TSubclassOf<URichTextBlockDecorator> DecoratorClass : DecoratorUMGClasses)
	{
		if (UClass* ResolvedClass = DecoratorClass.Get())
		{
			if (!ResolvedClass->HasAnyClassFlags(CLASS_Abstract))
			{
				URichTextBlockDecorator* Decorator = NewObject<URichTextBlockDecorator>(nullptr, ResolvedClass);
				InstanceUMGDecorators.Add(Decorator);
			}
		}
	}

	for (URichTextBlockDecorator* Decorator : InstanceUMGDecorators)
	{
		if (Decorator)
		{
			TSharedPtr<ITextDecorator> TextDecorator = Decorator->CreateDecorator(RichTextBlock);
			if (TextDecorator.IsValid())
			{
				OutDecorators.Add(TextDecorator.ToSharedRef());
			}
		}
	}
}

void SEdNode_Dialogue_Basic::MakeSlateDecoInstance(TArray<TSharedRef<class ITextDecorator>>& OutDecorators)
{
	TArray<TSubclassOf<USRichTextBlockDecorator>> DecoraterSlateClasses;

	for (TSubclassOf<USRichTextBlockDecorator> DecoratorClass : DecoraterSlateClasses)
	{
		if (UClass* ResolvedClass = DecoratorClass.Get())
		{
			if (!ResolvedClass->HasAnyClassFlags(CLASS_Abstract))
			{
				USRichTextBlockDecorator* Decorator = NewObject<USRichTextBlockDecorator>(nullptr, ResolvedClass);
				InstanceSlateDecorators.Add(Decorator);
			}
		}
	}

	for (USRichTextBlockDecorator* Decorator : InstanceSlateDecorators)
	{
		if (Decorator)
		{
			TSharedPtr<ITextDecorator> TextDecorator = Decorator->CreateDecorator(DialogueDefaultTextStyle);
			if (TextDecorator.IsValid())
			{
				OutDecorators.Add(TextDecorator.ToSharedRef());
			}
		}
	}
}

void SEdNode_Dialogue_Basic::UpdateRichTextStyle()
{
	ensure(DialogueStringTextBlock.IsValid());

	MakeStyleInstance();
	TArray<TSharedRef<ITextDecorator>> Decorators;
	MakeDecoInstance(Decorators);

	DialogueStringTextBlock->SetDecoratorStyleSet(StyleInstance.Get());
	DialogueStringTextBlock->SetDecorators(Decorators);
	DialogueStringTextBlock->SetTextStyle(DialogueDefaultTextStyle);
}


/////////////////////////////////////////////////////////////////////////


TSharedPtr<SCompoundWidget> SEdNode_Dialogue_Basic::CreateNodeBody()
{
	TSharedPtr<SBorder> NodeBody;

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	const FSlateBrush* NodeTypeIcon = GetNameIcon();
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, EdNode);
	//Create Body
	SAssignNew(NodeBody, SBorder)
		.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
		.BorderBackgroundColor(TitleShadowColor)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Visibility(EVisibility::SelfHitTestInvisible)
		.Padding(6.0f)
		[
			SNew(SBox)
			.MinDesiredWidth(80)
			[
				SNew(SVerticalBox)
					// Title
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					// Error message
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(ErrorReporting, SErrorText)
						.BackgroundColor(this, &SEdNode_Dialogue_Basic::GetErrorColor)
						.ToolTipText(this, &SEdNode_Dialogue_Basic::GetErrorMsgToolTip)
					]

					// Body
					+ SHorizontalBox::Slot()
					.Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
					[
						SNew(SVerticalBox)
						//Dialoguer Name
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SAssignNew(InlineEditableText, SInlineEditableTextBlock)
							.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
							.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
							.OnVerifyTextChanged(this, &SEdNode_Dialogue_Basic::OnVerifyNameTextChanged)
							.OnTextCommitted(this, &SEdNode_DialogueNode::OnNameTextCommited)
							.IsReadOnly(this, &SEdNode_DialogueNode::IsNameReadOnly)
							.IsSelected(this, &SEdNode_Dialogue_Basic::IsSelectedExclusively)
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							NodeTitle.ToSharedRef()
						]
					
					]
				]
				+ SVerticalBox::Slot()
				.Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.AutoHeight()
				[
					SAssignNew(OriginalDialogueStringTextBlock, STextBlock)
					.Visibility(this, &SEdNode_Dialogue_Basic::CanVisibleOriginalDialogue)
					.Text(this, &SEdNode_Dialogue_Basic::GetOriginalDialogueString)
					.WrapTextAt(1000)
					.Margin(FMargin(6.0f, 4.0f, 6.0f, 2.0f))
				]
				+ SVerticalBox::Slot()
				.Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.AutoHeight()
				[
					SAssignNew(DialogueStringTextBlock, SRichTextBlock)
					.Visibility(this, &SEdNode_Dialogue_Basic::CanVisibleDialogue)
					.Text(this, &SEdNode_Dialogue_Basic::GetDialogueString)
					.WrapTextAt(1000)
					.Margin(FMargin(6.0f, 4.0f, 6.0f, 2.0f))
				]

				+ SVerticalBox::Slot()
				.Padding(FMargin(8.0f, 3.0f, 8.0f, 3.0f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.AutoHeight()
				.SizeParam(FStretch(1000))
				[
					SAssignNew(DialogueImportBTN, SButton)
					.Visibility(this, &SEdNode_Dialogue_Basic::CanVisibleImportBTN)
					.OnClicked(this, &SEdNode_Dialogue_Basic::ImportDialogue)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString("Import")))
						.Justification(ETextJustify::Center)
					]
				]
			]
		];

	return NodeBody;
}



END_SLATE_FUNCTION_BUILD_OPTIMIZATION


