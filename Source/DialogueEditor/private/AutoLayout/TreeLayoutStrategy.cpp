#include "AutoLayout/TreeLayoutStrategy.h"
#include "DialogueEditor.h"
#include "DialogueHeaders.h"
#include "DialogueAssetEditor/SEdNode_DialogueNode.h"

UTreeLayoutStrategy::UTreeLayoutStrategy()
{
}

UTreeLayoutStrategy::~UTreeLayoutStrategy()
{

}

void UTreeLayoutStrategy::Layout(UEdGraph* _EdGraph)
{
	EdGraph = Cast<UEdGraph_DialogueSession>(_EdGraph);
	check(EdGraph != nullptr);

	EdGraph->RebuildDialogueSession();
	Graph = EdGraph->GetDialogueSession();
	check(Graph != nullptr);

	bool bFirstPassOnly = false;

	if (Settings != nullptr)
	{
		OptimalDistance = Settings->OptimalDistance;
		MaxIteration = Settings->MaxIteration;
		bFirstPassOnly = Settings->bFirstPassOnly;
	}

	FVector2D Anchor(0.f, 0.f);

	UDialogueNode* StartNode = Cast<UDialogueNode>(Graph->StartNode);
	InitPass(StartNode, Anchor);

	if (!bFirstPassOnly)
	{
		for (int32 j = 0; j < MaxIteration; ++j)
		{
			bool HasConflict = ResolveConflictPass(StartNode);
			if (!HasConflict)
			{
				break;
			}
		}
	}
}

void UTreeLayoutStrategy::InitPass(UDialogueNode* RootNode, const FVector2D& Anchor)
{
	UEdGraphNode_Dialogue* EdNode_RootNode = EdGraph->NodeMap[RootNode];

	FVector2D ChildAnchor(FVector2D(0.f, GetNodeHeight(EdNode_RootNode) + OptimalDistance + Anchor.Y));
	for (int32 i = 0; i < RootNode->ChildrenNodes.Num(); ++i)
	{
		UDialogueNode* Child = RootNode->ChildrenNodes[i];
		UEdGraphNode_Dialogue* EdNode_ChildNode = EdGraph->NodeMap[Child];
		if (i > 0)
		{
			UDialogueNode* PreChild = RootNode->ChildrenNodes[i - 1];
			UEdGraphNode_Dialogue* EdNode_PreChildNode = EdGraph->NodeMap[PreChild];
			ChildAnchor.X += OptimalDistance + GetNodeWidth(EdNode_PreChildNode) / 2;
		}
		ChildAnchor.X += GetNodeWidth(EdNode_ChildNode) / 2;
		InitPass(Child, ChildAnchor);
	}
	
	float NodeWidth = GetNodeWidth(EdNode_RootNode);

	EdNode_RootNode->NodePosY = Anchor.Y;
	if (RootNode->ChildrenNodes.Num() == 0)
	{
		EdNode_RootNode->NodePosX = Anchor.X - NodeWidth / 2;
	}
	else
	{
		UpdateParentNodePosition(RootNode);
	}
}

bool UTreeLayoutStrategy::ResolveConflictPass(UDialogueNode* Node)
{
	bool HasConflict = false;
	for (int32 i = 0; i < Node->ChildrenNodes.Num(); ++i)
	{
		UDialogueNode* Child = Node->ChildrenNodes[i];
		if (ResolveConflictPass(Child))
		{
			HasConflict = true;
		}
	}

	for (int32 i = 0; i < Node->ParentNodes.Num(); ++i)
	{
		UDialogueNode* ParentNode = Node->ParentNodes[i];
		for (int32 j = 0; j < ParentNode->ChildrenNodes.Num(); ++j)
		{
			UDialogueNode* LeftSibling = ParentNode->ChildrenNodes[j];
			if (LeftSibling == Node)
				break;
			if (ResolveConflict(LeftSibling, Node))
			{
				HasConflict = true;
			}
		}
	}

	return HasConflict;
}

bool UTreeLayoutStrategy::ResolveConflict(UDialogueNode* LRoot, UDialogueNode* RRoot)
{
	TArray<UEdGraphNode_Dialogue*> RightContour, LeftContour;

	GetRightContour(LRoot, 0, RightContour);
	GetLeftContour(RRoot, 0, LeftContour);

	int32 MaxOverlapDistance = 0;
	int32 Num = FMath::Min(LeftContour.Num(), RightContour.Num());
	for (int32 i = 0; i < Num; ++i)
	{
		if (RightContour.Contains(LeftContour[i]) || LeftContour.Contains(RightContour[i]))
			break;

		int32 RightBound = RightContour[i]->NodePosX + GetNodeWidth(RightContour[i]);
		int32 LeftBound = LeftContour[i]->NodePosX;
		int32 Distance = RightBound + OptimalDistance - LeftBound;
		if (Distance > MaxOverlapDistance)
		{
			MaxOverlapDistance = Distance;
		}
	}

	if (MaxOverlapDistance > 0)
	{
		ShiftSubTree(RRoot, FVector2D(MaxOverlapDistance, 0.f));

		TArray<UDialogueNode*> ParentNodes = RRoot->ParentNodes;
		TArray<UDialogueNode*> NextParentNodes;
		while (ParentNodes.Num() != 0)
		{
			for (int32 i = 0; i < ParentNodes.Num(); ++i)
			{
				UpdateParentNodePosition(ParentNodes[i]);

				NextParentNodes.Append(ParentNodes[i]->ParentNodes);
			}

			ParentNodes = NextParentNodes;
			NextParentNodes.Reset();
		}

		return true;
	}
	else
	{
		return false;
	}
}

void UTreeLayoutStrategy::GetLeftContour(UDialogueNode* RootNode, int32 Level, TArray<UEdGraphNode_Dialogue*>& Contour)
{
	UEdGraphNode_Dialogue* EdNode_Node = EdGraph->NodeMap[RootNode];
	if (Level >= Contour.Num())
	{
		Contour.Add(EdNode_Node);
	}
	else if (EdNode_Node->NodePosX < Contour[Level]->NodePosX)
	{
		Contour[Level] = EdNode_Node;
	}

	for (int32 i = 0; i < RootNode->ChildrenNodes.Num(); ++i)
	{
		GetLeftContour(RootNode->ChildrenNodes[i], Level + 1, Contour);
	}
}

void UTreeLayoutStrategy::GetRightContour(UDialogueNode* RootNode, int32 Level, TArray<UEdGraphNode_Dialogue*>& Contour)
{
	UEdGraphNode_Dialogue* EdNode_Node = EdGraph->NodeMap[RootNode];
	if (Level >= Contour.Num())
	{
		Contour.Add(EdNode_Node);
	}
	else if (EdNode_Node->NodePosX + GetNodeWidth(EdNode_Node) > Contour[Level]->NodePosX + GetNodeWidth(Contour[Level]))
	{
		Contour[Level] = EdNode_Node;
	}

	for (int32 i = 0; i < RootNode->ChildrenNodes.Num(); ++i)
	{
		GetRightContour(RootNode->ChildrenNodes[i], Level + 1, Contour);
	}
}

void UTreeLayoutStrategy::ShiftSubTree(UDialogueNode* RootNode, const FVector2D& Offset)
{
	UEdGraphNode_Dialogue* EdNode_Node = EdGraph->NodeMap[RootNode];
	EdNode_Node->NodePosX += Offset.X;
	EdNode_Node->NodePosY += Offset.Y;

	for (int32 i = 0; i < RootNode->ChildrenNodes.Num(); ++i)
	{
		UDialogueNode* Child = RootNode->ChildrenNodes[i];

		if (Child->ParentNodes[0] == RootNode)
		{
			ShiftSubTree(RootNode->ChildrenNodes[i], Offset);
		}
	}
}

void UTreeLayoutStrategy::UpdateParentNodePosition(UDialogueNode* ParentNode)
{
	UEdGraphNode_Dialogue* EdNode_ParentNode = EdGraph->NodeMap[ParentNode];
	if (ParentNode->ChildrenNodes.Num() % 2 == 0)
	{
		UEdGraphNode_Dialogue* FirstChild = EdGraph->NodeMap[ParentNode->ChildrenNodes[0]];
		UEdGraphNode_Dialogue* LastChild = EdGraph->NodeMap[ParentNode->ChildrenNodes.Last()];
		float LeftBound = FirstChild->NodePosX;
		float RightBound = LastChild->NodePosX + GetNodeWidth(LastChild);
		EdNode_ParentNode->NodePosX = (LeftBound + RightBound) / 2 - GetNodeWidth(EdNode_ParentNode) / 2;
	}
	else
	{
		UEdGraphNode_Dialogue* MidChild = EdGraph->NodeMap[ParentNode->ChildrenNodes[ParentNode->ChildrenNodes.Num() / 2]];
		EdNode_ParentNode->NodePosX = MidChild->NodePosX + GetNodeWidth(MidChild) / 2 - GetNodeWidth(EdNode_ParentNode) / 2;
	}
}
