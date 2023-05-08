// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/SEdNode_DialogueNode.h"
#include "DialogueEditor.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
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
#include "SEdNode_DialogueNode.h"
#include "DialogueAssetEditor/SEdNode_DialogueNode.h"
#include "DialogueEditorHeaders.h"
#include "DialogueHeaders.h"
#include "Styling/AppStyle.h"
#include "Components/RichTextBlock.h"


#define LOCTEXT_NAMESPACE "EdGraphNode_DialogueNode"

class SDialoguePin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SDialoguePin) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		this->SetCursor(EMouseCursor::Default);

		bShowLabel = true;

		GraphPinObj = InPin;
		check(GraphPinObj != nullptr);

		const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
		check(Schema);

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SDialoguePin::GetPinBorder)
			.BorderBackgroundColor(this, &SDialoguePin::GetPinColor)
			.OnMouseButtonDown(this, &SDialoguePin::OnPinMouseDown)
			.Cursor(this, &SDialoguePin::GetPinCursor)
			.Padding(FMargin(5.0f))
		);
	}

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return DialogueColors::Pin::Default;
	}

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override
	{
		return SNew(STextBlock);
	}

	const FSlateBrush* GetPinBorder() const
	{
		return FAppStyle::GetBrush(TEXT("Graph.StateNode.Body"));
	}

	virtual TSharedRef<FDragDropOperation> SpawnPinDragEvent(const TSharedRef<class SGraphPanel>& InGraphPanel, const TArray< TSharedRef<SGraphPin> >& InStartingPins) override
	{
		FDialogueDragConnection::FDraggedPinTable PinHandles;
		PinHandles.Reserve(InStartingPins.Num());
		// since the graph can be refreshed and pins can be reconstructed/replaced 
		// behind the scenes, the DragDropOperation holds onto FGraphPinHandles 
		// instead of direct widgets/graph-pins
		for (const TSharedRef<SGraphPin>& PinWidget : InStartingPins)
		{
			PinHandles.Add(PinWidget->GetPinObj());
		}

		return FDialogueDragConnection::New(InGraphPanel, PinHandles);
	}

};



BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SEdNode_DialogueNode::UpdateGraphNode()
{
	const FMargin NodePadding = FMargin(5);
	const FMargin NamePadding = FMargin(2);

	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	TSharedPtr<SCompoundWidget> NodeBody = CreateNodeBody().ToSharedRef();
	

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(this, &SEdNode_DialogueNode::GetBorderBackgroundColor)
			[
				SNew(SOverlay)
					+ SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SVerticalBox)

							// Input Pin Area
							+ SVerticalBox::Slot()
							.FillHeight(1)
							[
								SAssignNew(LeftNodeBox, SVerticalBox)
							]

						// Output Pin Area	
							+ SVerticalBox::Slot()
							.FillHeight(1)
							[
								SAssignNew(RightNodeBox, SVerticalBox)
							]
					]

					+ SOverlay::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Padding(8.0f)
					[
						NodeBody.ToSharedRef()
					]
			]
		];


	// Create comment bubble
	CreateCommentBubble();
	
	CreatePinWidgets();

	SetupErrorReporting();
}


void SEdNode_DialogueNode::CreatePinWidgets()
{
	UEdGraphNode_Dialogue* StateNode = EdNode;

	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SDialoguePin, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SEdNode_DialogueNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	TSharedPtr<SVerticalBox> PinBox;
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		PinBox = LeftNodeBox;
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		PinBox = RightNodeBox;
		OutputPins.Add(PinToAdd);
	}

	if (PinBox)
	{
		PinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			//.Padding(6.0f, 0.0f)
			[
				PinToAdd
			];
	}
}

bool SEdNode_DialogueNode::IsNameReadOnly() const
{
	UEdGraphNode_Dialogue* EdNode_Node = EdNode;
	check(EdNode_Node != nullptr);

	UDialogueSession* DialogueSession = EdNode_Node->DialogueNode->DialogueSession;
	check(DialogueSession != nullptr);

	return (!DialogueSession->bCanRenameNode || !EdNode_Node->DialogueNode->IsNameEditable()) || SGraphNode::IsNameReadOnly();
}

TSharedPtr<SCommentBubble> SEdNode_DialogueNode::CreateCommentBubble()
{
	TSharedPtr<SCommentBubble> CommentBubble;
	const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;
	
	SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(EdNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(CommentColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];

	return CommentBubble;
}

TSharedPtr<SCompoundWidget> SEdNode_DialogueNode::CreateNodeBody()
{
	TSharedPtr<SBorder> NodeBody;

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
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
				.BackgroundColor(this, &SEdNode_DialogueNode::GetErrorColor)
				.ToolTipText(this, &SEdNode_DialogueNode::GetErrorMsgToolTip)
			]
				// Icon
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(NodeTypeIcon)
			]

			// Body
			+ SHorizontalBox::Slot()
			.Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
			[
				SNew(SVerticalBox)
				
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(InlineEditableText, SInlineEditableTextBlock)
					.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
					.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
					.OnVerifyTextChanged(this, &SEdNode_DialogueNode::OnVerifyNameTextChanged)
					.OnTextCommitted(this, &SEdNode_DialogueNode::OnNameTextCommited)
					.IsReadOnly(this, &SEdNode_DialogueNode::IsNameReadOnly)
					.IsSelected(this, &SEdNode_DialogueNode::IsSelectedExclusively)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					NodeTitle.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.AutoHeight()
				[
					SAssignNew(DescriptionTextBlock, STextBlock)
					.Visibility(this, &SEdNode_DialogueNode::GetDescriptionVisibility)
					.Text(this, &SEdNode_DialogueNode::GetDescription)
					//.WrapTextAt(5)
					.Margin(FMargin(6.0f, 4.0f, 6.0f, 1.0f))
				]
			]
		]
	];

	return NodeBody;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdNode_DialogueNode::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	UEdGraphNode_Dialogue* MyNode = EdNode;

	if (MyNode != nullptr && MyNode->DialogueNode != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("GenericGraphEditorRenameNode", "Generic Graph Editor: Rename Node"));
		MyNode->Modify();
		MyNode->DialogueNode->Modify();
		MyNode->DialogueNode->SetNodeTitle(InText);
		UpdateGraphNode();
	}
}

FSlateColor SEdNode_DialogueNode::GetBorderBackgroundColor() const
{
	UEdGraphNode_Dialogue* MyNode = EdNode;
	return MyNode ? MyNode->GetBackgroundColor() : DialogueColors::NodeBorder::HighlightAbortRange0;
}

FSlateColor SEdNode_DialogueNode::GetBackgroundColor() const
{
	return DialogueColors::NodeBody::Default;
}

EVisibility SEdNode_DialogueNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

const FSlateBrush* SEdNode_DialogueNode::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

FText SEdNode_DialogueNode::GetDescription() const
{
	return IsValid(EdNode) ? EdNode->GetNodeDescription() : FText::FromString("Can't Find EdGraphNode.");
}


void SEdNode_DialogueNode::Construct(const FArguments& InArgs, UEdGraphNode_Dialogue* InNode)
{
	EdNode = InNode;
	GraphNode = InNode;
	UpdateGraphNode();
	InNode->SEdNode = this;
}


#undef LOCTEXT_NAMESPACE