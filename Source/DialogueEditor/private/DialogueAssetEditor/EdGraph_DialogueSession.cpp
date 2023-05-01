// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/EdGraph_DialogueSession.h"
#include "DialogueEditor.h"
#include "DialogueSession.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
#include "DialogueAssetEditor/EdGraphNode_DialogueEdge.h"
#include "DialogueNode.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue_Basic.h"
#include "Settings_DialogueEditor.h"

UEdGraph_DialogueSession::UEdGraph_DialogueSession()
{
}


UEdGraph_DialogueSession::~UEdGraph_DialogueSession()
{
}

void UEdGraph_DialogueSession::RebuildDialogueSession()
{
	LOG_INFO(TEXT("has been called"));

	UDialogueSession* GraphSession = GetDialogueSession();

	Clear();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		//��� ������ �۾�
		if (UEdGraphNode_Dialogue* EdNode = Cast<UEdGraphNode_Dialogue>(Nodes[i]))
		{
			if (EdNode->DialogueNode == nullptr) continue;

			UDialogueNode* CustomNode = EdNode->DialogueNode;

			NodeMap.Add(CustomNode, EdNode);

			GraphSession->AllNodes.Add(CustomNode);

			for (int PinIdx = 0; PinIdx < EdNode->Pins.Num(); ++PinIdx)
			{
				UEdGraphPin* Pin = EdNode->Pins[PinIdx];
				if(Pin->Direction != EEdGraphPinDirection::EGPD_Output) continue;	//������� �ƴϸ� ��������

				for (int LinkToIdx = 0; LinkToIdx < Pin->LinkedTo.Num(); ++LinkToIdx)
				{
					UDialogueNode* ChildNode = nullptr;

					//���� ���� ����Ǿ� ������
					if (UEdGraphNode_Dialogue* EdNode_Child = Cast<UEdGraphNode_Dialogue>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						ChildNode = EdNode_Child->DialogueNode;
					}
					//���� ����Ǿ��ִ� ���� Edge�� Edge�� ���� �ڽ� ��带 Ž��
					else if(UEdGraphNode_DialogueEdge* EdNode_Edge = Cast<UEdGraphNode_DialogueEdge>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						UEdGraphNode_Dialogue* ChildEdNodeFromEdge = EdNode_Edge->GetEndNode();
						if (ChildEdNodeFromEdge != nullptr)
						{
							ChildNode = ChildEdNodeFromEdge->DialogueNode;
						}
					}

					//Ž���ؼ� ã�Ƴ´ٸ�
					if (ChildNode != nullptr)
					{
						CustomNode->ChildrenNodes.Add(ChildNode);
						ChildNode->ParentNodes.Add(CustomNode);
					}
					else
					{
						LOG_ERROR(TEXT("Can't find Child Node"));
					}
				}
			}

			CustomNode->RebuildNode();
		}
		//���� ������ �۾�
		else if (UEdGraphNode_DialogueEdge* EdgeNode = Cast<UEdGraphNode_DialogueEdge>(Nodes[i]))
		{
			UEdGraphNode_Dialogue* StartNode = EdgeNode->GetStartNode();
			UEdGraphNode_Dialogue* EndNode = EdgeNode->GetEndNode();
			UDialogueEdge* Edge = EdgeNode->DialogueEdge;

			if (StartNode == nullptr || EndNode == nullptr || Edge == nullptr)
			{
				LOG_ERROR(TEXT("Add edge failed."));
				continue;
			}

			EdgeMap.Add(Edge, EdgeNode);

			Edge->DialogueSession = GraphSession;
			Edge->Rename(nullptr, GraphSession, REN_DontCreateRedirectors | REN_DoNotDirty);
			Edge->StartNode = StartNode->DialogueNode;
			Edge->EndNode = EndNode->DialogueNode;
			Edge->StartNode->Edges.Add(Edge->EndNode, Edge);
		}
	}

	for (int i = 0; i < GraphSession->AllNodes.Num(); ++i)
	{
		UDialogueNode* Node = GraphSession->AllNodes[i];
		SortNode(Node);

		Node->DialogueSession = GraphSession;
		Node->Rename(nullptr, GraphSession, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

UDialogueSession* UEdGraph_DialogueSession::GetDialogueSession() const
{
	return CastChecked<UDialogueSession>(GetOuter());
}

bool UEdGraph_DialogueSession::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool FinalValue = Super::Modify(bAlwaysMarkDirty);

	GetDialogueSession()->Modify();

	for (int32 i = 0; i <  Nodes.Num(); ++i)
	{
		Nodes[i]->Modify();
	}

	return FinalValue;
}

void UEdGraph_DialogueSession::PostLoad()
{
	Super::PostLoad();
}

void UEdGraph_DialogueSession::Clear()
{
	UDialogueSession* GraphSession = GetDialogueSession();

	GraphSession->ClearGraph();
	NodeMap.Reset();
	EdgeMap.Reset();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (UEdGraphNode_Dialogue* EdNode = Cast<UEdGraphNode_Dialogue>(Nodes[i]))
		{
			UDialogueNode* CustomNode = EdNode->DialogueNode;
			if (CustomNode)
			{
				CustomNode->ParentNodes.Reset();
				CustomNode->ChildrenNodes.Reset();
				CustomNode->Edges.Reset();
			}
		}
	}
}

void UEdGraph_DialogueSession::SortNode(UDialogueNode* TargetNode)
{
	UDialogueNode* Node = TargetNode;

	auto Comp = [&](const UDialogueNode& L, const UDialogueNode& R)
	{
		UEdGraphNode_Dialogue* EdNode_LNode = NodeMap[&L];
		UEdGraphNode_Dialogue* EdNode_RNode = NodeMap[&R];
		return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
	};

	Node->ChildrenNodes.Sort(Comp);
	Node->ParentNodes.Sort(Comp);
}



void UEdGraph_DialogueSession::PostEditUndo()
{
	Super::PostEditUndo();

	NotifyGraphChanged();
}