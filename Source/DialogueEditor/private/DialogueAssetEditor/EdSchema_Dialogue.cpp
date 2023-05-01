// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/EdSchema_Dialogue.h"
#include "DialogueEditor.h"
#include "Nodes/DialogueNode_Start.h"
#include "DialogueAssetEditor/EdGraph_DialogueSession.h"
#include "DialogueSession.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
#include "Framework/Commands/GenericCommands.h"
#include "ToolMenus.h"
#include "AutoLayout/ForceDirectedLayoutStrategy.h"
#include "AutoLayout/TreeLayoutStrategy.h"
#include "GraphEditorActions.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
#include "DialogueEditorHeaders.h"
#include "DialogueAssetEditor/ConnectionDrawingPolicy_Dialogue.h"



#define LOCTEXT_NAMESPACE "EdSchema_Dialogue"


int32 UEdSchema_Dialogue::CurrentCacheRefreshID = 0;

class FNodeVisitorCycleChecker
{
public:
	/** Check whether a loop in the graph would be caused by linking the passed-in nodes */
	bool CheckForLoop(UEdGraphNode* StartNode, UEdGraphNode* EndNode)
	{
		VisitedNodes.Add(StartNode);

		return TraverseNodes(EndNode);
	}

private:
	bool TraverseNodes(UEdGraphNode* Node)
	{
		VisitedNodes.Add(Node);

		for (auto MyPin : Node->Pins)
		{
			if (MyPin->Direction == EGPD_Output)
			{
				for (auto OtherPin : MyPin->LinkedTo)
				{
					UEdGraphNode* OtherNode = OtherPin->GetOwningNode();
					if (VisitedNodes.Contains(OtherNode))
					{
						// Only  an issue if this is a back-edge
						return false;
					}
					else if (!FinishedNodes.Contains(OtherNode))
					{
						// Only should traverse if this node hasn't been traversed
						if (!TraverseNodes(OtherNode))
							return false;
					}
				}
			}
		}

		VisitedNodes.Remove(Node);
		FinishedNodes.Add(Node);
		return true;
	};


	TSet<UEdGraphNode*> VisitedNodes;
	TSet<UEdGraphNode*> FinishedNodes;
};

UEdGraphNode* FEdSchemaAction_Dialogue_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("DialogueEditorNewNode", "Custom Graph Editor : New Node"));
		ParentGraph->Modify();
		if (FromPin != nullptr) FromPin->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->DialogueNode->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FEdSchemaAction_Dialogue_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);

}

UEdGraphNode* FEdSchemaAction_Dialogue_NewEdge::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("DialogueEditorNewNode", "Custom Graph Editor : New Edge"));
		ParentGraph->Modify();
		if (FromPin != nullptr) FromPin->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->DialogueEdge->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FEdSchemaAction_Dialogue_NewEdge::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);

}

void UEdSchema_Dialogue::GetBreakLinkToSubMenuActions(class UToolMenu* Menu, class UEdGraphPin* InGraphPin)
{
	// Make sure we have a unique name for every entry in the list
	TMap< FString, uint32 > LinkTitleCount;

	FToolMenuSection& Section = Menu->FindOrAddSection("DialogueSchemaPinActions");

	// Add all the links we could break from
	for (TArray<class UEdGraphPin*>::TConstIterator Links(InGraphPin->LinkedTo); Links; ++Links)
	{
		UEdGraphPin* Pin = *Links;
		FString TitleString = Pin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString();
		FText Title = FText::FromString(TitleString);
		if (Pin->PinName != TEXT(""))
		{
			TitleString = FString::Printf(TEXT("%s (%s)"), *TitleString, *Pin->PinName.ToString());

			// Add name of connection if possible
			FFormatNamedArguments Args;
			Args.Add(TEXT("NodeTitle"), Title);
			Args.Add(TEXT("PinName"), Pin->GetDisplayName());
			Title = FText::Format(LOCTEXT("BreakDescPin", "{NodeTitle} ({PinName})"), Args);
		}

		uint32& Count = LinkTitleCount.FindOrAdd(TitleString);

		FText Description;
		FFormatNamedArguments Args;
		Args.Add(TEXT("NodeTitle"), Title);
		Args.Add(TEXT("NumberOfNodes"), Count);

		if (Count == 0)
		{
			Description = FText::Format(LOCTEXT("BreakDesc", "Break link to {NodeTitle}"), Args);
		}
		else
		{
			Description = FText::Format(LOCTEXT("BreakDescMulti", "Break link to {NodeTitle} ({NumberOfNodes})"), Args);
		}
		++Count;

		Section.AddMenuEntry(NAME_None, Description, Description, FSlateIcon(), FUIAction(
			FExecuteAction::CreateUObject(this, &UEdSchema_Dialogue::BreakSinglePinLink, const_cast<UEdGraphPin*>(InGraphPin), *Links)));
	}
}

EGraphType UEdSchema_Dialogue::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return EGraphType::GT_StateMachine;
}




const FPinConnectionResponse UEdSchema_Dialogue::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Make sure the pins are not on the same node
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Can't connect node to itself"));
	}

	const UEdGraphPin* Out = A;
	const UEdGraphPin* In = B;

	UEdGraphNode_Dialogue* EdNode_Out = Cast<UEdGraphNode_Dialogue>(Out->GetOwningNode());
	UEdGraphNode_Dialogue* EdNode_In = Cast<UEdGraphNode_Dialogue>(In->GetOwningNode());

	if (EdNode_Out == nullptr || EdNode_In == nullptr)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinError", "Not a valid UDialogueEdNode"));
	}

	//Determine if we can have cycles or not
	bool bAllowCycles = false;
	auto EdGraph = Cast<UEdGraph_DialogueSession>(Out->GetOwningNode()->GetGraph());
	if (EdGraph != nullptr)
	{
		bAllowCycles = EdGraph->GetDialogueSession()->bCanBeCyclical;
	}

	// check for cycles
	FNodeVisitorCycleChecker CycleChecker;
	if (!bAllowCycles && !CycleChecker.CheckForLoop(Out->GetOwningNode(), In->GetOwningNode()))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorCycle", "Can't create a graph cycle"));
	}

	FText ErrorMessage;
	if (!EdNode_Out->DialogueNode->CanCreateConnectionTo(EdNode_In->DialogueNode, EdNode_Out->GetOutputPin()->LinkedTo.Num(), ErrorMessage))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	}
	if (!EdNode_In->DialogueNode->CanCreateConnectionFrom(EdNode_Out->DialogueNode, EdNode_In->GetInputPin()->LinkedTo.Num(), ErrorMessage))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	}


	if (EdNode_Out->DialogueNode->GetDialogueSession()->bEdgeEnabled)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, LOCTEXT("PinConnect", "Connect nodes with edge"));
	}
	else
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes"));
	}
}

bool UEdSchema_Dialogue::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	UEdGraphNode_Dialogue* NodeA = Cast<UEdGraphNode_Dialogue>(A->GetOwningNode());
	UEdGraphNode_Dialogue* NodeB = Cast<UEdGraphNode_Dialogue>(B->GetOwningNode());

	// Check that this edge doesn't already exist
	for (UEdGraphPin* TestPin : NodeA->GetOutputPin()->LinkedTo)
	{
		UEdGraphNode* ChildNode = TestPin->GetOwningNode();
		if (UEdGraphNode_DialogueEdge* EdNode_Edge = Cast<UEdGraphNode_DialogueEdge>(ChildNode))
		{
			ChildNode = EdNode_Edge->GetEndNode();
		}

		if (ChildNode == NodeB)
			return false;
	}

	if (NodeA && NodeB)
	{
		// Always create connections from node A to B, don't allow adding in reverse
		Super::TryCreateConnection(NodeA->GetOutputPin(), NodeB->GetInputPin());
		return true;
	}
	else
	{
		return false;
	}
}

bool UEdSchema_Dialogue::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{
	UEdGraphNode_Dialogue* NodeA = Cast<UEdGraphNode_Dialogue>(A->GetOwningNode());
	UEdGraphNode_Dialogue* NodeB = Cast<UEdGraphNode_Dialogue>(B->GetOwningNode());

	// Are nodes and pins all valid?
	if (!NodeA || !NodeA->GetOutputPin() || !NodeB || !NodeB->GetInputPin())
		return false;

	UDialogueSession* Graph = NodeA->DialogueNode->GetDialogueSession();

	FVector2D InitPos((NodeA->NodePosX + NodeB->NodePosX) / 2, (NodeA->NodePosY + NodeB->NodePosY) / 2);

	FEdSchemaAction_Dialogue_NewEdge Action;
	Action.NodeTemplate = NewObject<UEdGraphNode_DialogueEdge>(NodeA->GetGraph());
	Action.NodeTemplate->SetEdge(NewObject<UDialogueEdge>(Action.NodeTemplate, UDialogueEdge::StaticClass()));
	UEdGraphNode_DialogueEdge* EdgeNode = Cast<UEdGraphNode_DialogueEdge>(Action.PerformAction(NodeA->GetGraph(), nullptr, InitPos, false));

	// Always create connections from node A to B, don't allow adding in reverse
	EdgeNode->CreateConnections(NodeA, NodeB);

	return true;
}

FConnectionDrawingPolicy* UEdSchema_Dialogue::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return new FConnectionDrawingPolicy_Dialogue(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

FLinearColor UEdSchema_Dialogue::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::White;
}

void UEdSchema_Dialogue::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);

}

void UEdSchema_Dialogue::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UEdSchema_Dialogue::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));

	Super::BreakSinglePinLink(SourcePin, TargetPin);

}

UEdGraphPin* UEdSchema_Dialogue::DropPinOnNode(UEdGraphNode* InTargetNode, const FName& InSourcePinName, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection) const
{
	UEdGraphNode_Dialogue* EdNode = Cast<UEdGraphNode_Dialogue>(InTargetNode);
	switch (InSourcePinDirection)
	{
	case EGPD_Input:
		return EdNode->GetOutputPin();
	case EGPD_Output:
		return EdNode->GetInputPin();
	default:
		return nullptr;
	}
}

bool UEdSchema_Dialogue::SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const
{
	return Cast<UEdGraphNode_Dialogue>(InTargetNode) != nullptr;
}

bool UEdSchema_Dialogue::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UEdSchema_Dialogue::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UEdSchema_Dialogue::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}




FText UEdSchema_Dialogue::GetNodeCategoryName() const
{
	return LOCTEXT("Default Dialogue NodeCategory", "Dialogue Node");
}

void UEdSchema_Dialogue::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	CreateStartNode(Graph);
}

void UEdSchema_Dialogue::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection("DialogueSchemaNodeActions", LOCTEXT("ClassActionsMenuHeader", "Node Actions"));

		const UEdGraphNode_Dialogue* EdNode = Cast<const UEdGraphNode_Dialogue>(Context->Node);
		if (EdNode)
		{
			const UDialogueNode* DialogueNode = Cast<const UDialogueNode>(EdNode->GetDialogueNode());
			if (DialogueNode->GetDialogueNodeType() != EDialogueNodeType::Start)
			{
				Section.AddMenuEntry(FGenericCommands::Get().Delete);
				Section.AddMenuEntry(FGenericCommands::Get().Cut);
				Section.AddMenuEntry(FGenericCommands::Get().Copy);
				Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
			}
		}

		Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
	}
}

void UEdSchema_Dialogue::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	UDialogueSession* DialogueSession = CastChecked<UDialogueSession>(ContextMenuBuilder.CurrentGraph->GetOuter());
	if (!IsValid(DialogueSession)) {
		LOG_ERROR(TEXT("Can't Cast DialogueSession."));
		return;
	}

	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);

	const FText AddToolTip = LOCTEXT("NewDialogueNodTooltip", "Add node here");

	TSet<TSubclassOf<UDialogueNode> > Visited;

	FText Desc;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (!It->HasAnyClassFlags(EClassFlags::CLASS_Abstract) && It->IsChildOf(UDialogueNode::StaticClass()) && !Visited.Contains(*It))
		{
			if(It->IsChildOf<UDialogueNode_Start>()) continue;

			TSubclassOf<UDialogueNode> NodeType = *It;

			if (It->GetName().StartsWith("REINST") || It->GetName().StartsWith("SKEL"))	continue;

			Desc = NodeType.GetDefaultObject()->ContextMenuName;

			if (Desc.IsEmpty())
			{
				FString Title = NodeType->GetDefaultObject()->GetName();
				Title.RemoveFromEnd("_C");
				Desc = FText::FromString(Title);
			}

			TSharedPtr<FEdSchemaAction_Dialogue_NewNode> ChildNewNodeAction(new FEdSchemaAction_Dialogue_NewNode(GetNodeCategoryName(), Desc, AddToolTip, 0));
			
			TSubclassOf<UEdGraphNode_Dialogue> EdNodeType = GetEdDialogueNodeSubclass(NodeType.GetDefaultObject()->GetDialogueNodeType());
			ChildNewNodeAction->NodeTemplate = NewObject<UEdGraphNode_Dialogue>(ContextMenuBuilder.OwnerOfTemporaries, EdNodeType);
			ChildNewNodeAction->NodeTemplate->SetDialogueNode(NewObject<UDialogueNode>(ChildNewNodeAction->NodeTemplate, NodeType));
			ChildNewNodeAction->NodeTemplate->DialogueNode->DialogueSession = DialogueSession;
			ContextMenuBuilder.AddAction(ChildNewNodeAction);

			Visited.Add(NodeType);
		}
	}
}
void UEdSchema_Dialogue::CreateStartNode(UEdGraph& Graph) const
{
	UDialogueSession* DialogueSession = Cast<UDialogueSession>(Cast<UEdGraph_DialogueSession>(&Graph)->GetDialogueSession());
	if (!IsValid(DialogueSession)) {
		LOG_ERROR(TEXT("Can't Cast DialogueSession."));
		return ;
	}

	TSharedPtr<FEdSchemaAction_Dialogue_NewNode> NewNodeAction(new FEdSchemaAction_Dialogue_NewNode(GetNodeCategoryName(), FText(), FText(), 0));
	NewNodeAction->NodeTemplate = NewObject<UEdGraphNode_Dialogue_Start>(&Graph);
	NewNodeAction->NodeTemplate->SetDialogueNode(NewObject<UDialogueNode>(NewNodeAction->NodeTemplate, UDialogueNode_Start::StaticClass() ) );
	NewNodeAction->NodeTemplate->DialogueNode->DialogueSession = DialogueSession;
	UDialogueNode_Start* StartNode = Cast<UDialogueNode_Start>(NewNodeAction->NodeTemplate->DialogueNode);
	if(IsValid(StartNode))	{ StartNode->DialogueSession = DialogueSession; }
	UEdGraphNode* NewEdNode = NewNodeAction->PerformAction(&Graph, nullptr, FVector2D(0, 0));

	DialogueSession->StartNode = StartNode;
}

TSubclassOf<UEdGraphNode_Dialogue> UEdSchema_Dialogue::GetEdDialogueNodeSubclass(EDialogueNodeType NodeType) const
{
	TSubclassOf<UEdGraphNode_Dialogue> ReturnValue;

	switch (NodeType)
	{
		case EDialogueNodeType::Start:
			ReturnValue = UEdGraphNode_Dialogue_Start::StaticClass();
		break;

		case EDialogueNodeType::Basic:
			ReturnValue = UEdGraphNode_Dialogue_Basic::StaticClass();
		break;

		case EDialogueNodeType::Question:
		case EDialogueNodeType::Answer:
		case EDialogueNodeType::End:
		default:
			ReturnValue = UEdGraphNode_Dialogue::StaticClass();
		break;
	}

	return ReturnValue;
}

#undef LOCTEXT_NAMESPACE

