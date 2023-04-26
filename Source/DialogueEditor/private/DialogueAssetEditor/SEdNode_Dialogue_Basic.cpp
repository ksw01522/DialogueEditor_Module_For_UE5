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
#include "EditorStyleSet.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdNode_Dialogue_Basic::Construct(const FArguments& InArgs, UEdGraphNode_Dialogue_Basic* InNode)
{
	EdNode = InNode;
	GraphNode = InNode;
	UpdateGraphNode();
	InNode->SEdNode = this;
	EdNode_Basic = InNode;
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

TSharedPtr<SCompoundWidget> SEdNode_Dialogue_Basic::CreateNodeBody()
{
	TSharedPtr<SBorder> NodeBody;

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	const FSlateBrush* NodeTypeIcon = GetNameIcon();
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, EdNode);

	//Create Body
	SAssignNew(NodeBody, SBorder)
		.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.ColorSpill"))
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
						SAssignNew(ErrorText, SErrorText)
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
							.Style(FEditorStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
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
					SAssignNew(DialogueStringTextBlock, STextBlock)
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


	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);

	return NodeBody;
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION