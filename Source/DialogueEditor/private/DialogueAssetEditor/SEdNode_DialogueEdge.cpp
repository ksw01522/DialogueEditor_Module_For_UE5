// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/SEdNode_DialogueEdge.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/SToolTip.h"
#include "SGraphPanel.h"
#include "EdGraphSchema_K2.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
#include "DialogueAssetEditor/EdGraphNode_DialogueEdge.h"
#include "DialogueAssetEditor/ConnectionDrawingPolicy_Dialogue.h"
#include "DialogueAssetEditor/DialogueEditorStyle.h"

#define LOCTEXT_NAMESPACE "SDialogueEdge"

void SEdNode_DialogueEdge::Construct(const FArguments& InArgs, UEdGraphNode_DialogueEdge* InNode)
{
	this->GraphNode = InNode;
	this->UpdateGraphNode();
}

bool SEdNode_DialogueEdge::RequiresSecondPassLayout() const
{
	return true;
}

void SEdNode_DialogueEdge::PerformSecondPassLayout(const TMap<UObject*, TSharedRef<SNode>>& NodeToWidgetLookup) const
{
	UEdGraphNode_DialogueEdge* EdgeNode = CastChecked<UEdGraphNode_DialogueEdge>(GraphNode);

	FGeometry StartGeom;
	FGeometry EndGeom;

	UEdGraphNode_Dialogue* Start = EdgeNode->GetStartNode();
	UEdGraphNode_Dialogue* End = EdgeNode->GetEndNode();
	if (Start != nullptr && End != nullptr)
	{
		const TSharedRef<SNode>* pFromWidget = NodeToWidgetLookup.Find(Start);
		const TSharedRef<SNode>* pToWidget = NodeToWidgetLookup.Find(End);
		if (pFromWidget != nullptr && pToWidget != nullptr)
		{
			const TSharedRef<SNode>& FromWidget = *pFromWidget;
			const TSharedRef<SNode>& ToWidget = *pToWidget;

			StartGeom = FGeometry(FVector2D(Start->NodePosX, Start->NodePosY), FVector2D::ZeroVector, FromWidget->GetDesiredSize(), 1.0f);
			EndGeom = FGeometry(FVector2D(End->NodePosX, End->NodePosY), FVector2D::ZeroVector, ToWidget->GetDesiredSize(), 1.0f);
		}
	}

	PositionBetweenTwoNodesWithOffset(StartGeom, EndGeom, 0, 1);
}

void SEdNode_DialogueEdge::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	UEdGraphNode_DialogueEdge* MyNode = CastChecked<UEdGraphNode_DialogueEdge>(GraphNode);

	if (MyNode != nullptr && MyNode->DialogueEdge != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("GenericGraphEditorRenameEdge", "Generic Graph Editor: Rename Edge"));
		MyNode->Modify();
		MyNode->DialogueEdge->SetNodeTitle(InText);
		UpdateGraphNode();
	}
}

void SEdNode_DialogueEdge::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		[
			SNew(SImage)
			.Image(FAppStyle::GetBrush("Graph.TransitionNode.ColorSpill"))
		.ColorAndOpacity(this, &SEdNode_DialogueEdge::GetEdgeColor)
		]
	+ SOverlay::Slot()
		[
			SNew(SImage)
			.Image(this, &SEdNode_DialogueEdge::GetEdgeImage)
		.Visibility(this, &SEdNode_DialogueEdge::GetEdgeImageVisibility)
		]

	+ SOverlay::Slot()
		.Padding(FMargin(4.0f, 4.0f, 4.0f, 4.0f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			SAssignNew(InlineEditableText, SInlineEditableTextBlock)
			.ColorAndOpacity(FLinearColor::Black)
		.Visibility(this, &SEdNode_DialogueEdge::GetEdgeTitleVisbility)
		.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
		.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
		.OnTextCommitted(this, &SEdNode_DialogueEdge::OnNameTextCommited)
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		[
			NodeTitle.ToSharedRef()
		]

		]
		];
}

void SEdNode_DialogueEdge::PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	// Position ourselves halfway along the connecting line between the nodes, elevated away perpendicular to the direction of the line
	const float Height = 30.0f;

	const FVector2D DesiredNodeSize = GetDesiredSize();

	FVector2D DeltaPos(EndAnchorPoint - StartAnchorPoint);

	if (DeltaPos.IsNearlyZero())
	{
		DeltaPos = FVector2D(10.0f, 0.0f);
	}

	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	const FVector2D NewCenter = StartAnchorPoint + (0.5f * DeltaPos) + (Height * Normal);

	FVector2D DeltaNormal = DeltaPos.GetSafeNormal();

	// Calculate node offset in the case of multiple transitions between the same two nodes
	// MultiNodeOffset: the offset where 0 is the centre of the transition, -1 is 1 <size of node>
	// towards the PrevStateNode and +1 is 1 <size of node> towards the NextStateNode.

	const float MutliNodeSpace = 0.2f; // Space between multiple transition nodes (in units of <size of node> )
	const float MultiNodeStep = (1.f + MutliNodeSpace); //Step between node centres (Size of node + size of node spacer)

	const float MultiNodeStart = -((MaxNodes - 1) * MultiNodeStep) / 2.f;
	const float MultiNodeOffset = MultiNodeStart + (NodeIndex * MultiNodeStep);

	// Now we need to adjust the new center by the node size, zoom factor and multi node offset
	const FVector2D NewCorner = NewCenter - (0.5f * DesiredNodeSize) + (DeltaNormal * MultiNodeOffset * DesiredNodeSize.Size());

	GraphNode->NodePosX = NewCorner.X;
	GraphNode->NodePosY = NewCorner.Y;
}

FSlateColor SEdNode_DialogueEdge::GetEdgeColor() const
{
	UEdGraphNode_DialogueEdge* EdgeNode = CastChecked<UEdGraphNode_DialogueEdge>(GraphNode);
	if (EdgeNode != nullptr && EdgeNode->DialogueEdge != nullptr)
	{
		return EdgeNode->DialogueEdge->GetEdgeColor();
	}
	return FLinearColor(0.9f, 0.9f, 0.9f, 1.0f);
}

const FSlateBrush* SEdNode_DialogueEdge::GetEdgeImage() const
{
	return FAppStyle::GetBrush("Graph.TransitionNode.Icon");
}

EVisibility SEdNode_DialogueEdge::GetEdgeImageVisibility() const
{
	UEdGraphNode_DialogueEdge* EdgeNode = CastChecked<UEdGraphNode_DialogueEdge>(GraphNode);
	if (EdgeNode && EdgeNode->DialogueEdge && EdgeNode->DialogueEdge->bShouldDrawTitle)
		return EVisibility::Hidden;

	return EVisibility::Visible;
}

EVisibility SEdNode_DialogueEdge::GetEdgeTitleVisbility() const
{
	UEdGraphNode_DialogueEdge* EdgeNode = CastChecked<UEdGraphNode_DialogueEdge>(GraphNode);
	if (EdgeNode && EdgeNode->DialogueEdge && EdgeNode->DialogueEdge->bShouldDrawTitle)
		return EVisibility::Visible;

	return EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE