// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueNodeFactory.h"
#include <EdGraph/EdGraphNode.h>
#include "DialogueAssetEditor/SEdNode_DialogueEdge.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
#include "DialogueAssetEditor/SEdNode_DialogueNode.h"
#include "DialogueAssetEditor/EdGraphNode_DialogueEdge.h"
#include "DialogueNode.h"
#include "DialogueEditorHeaders.h"



TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_Dialogue::CreateNode(UEdGraphNode* Node) const
{
	if (UEdGraphNode_Dialogue* EdNode_GraphNode = Cast<UEdGraphNode_Dialogue>(Node))
	{
		if (UEdGraphNode_Dialogue* EdNode_Dialogue = Cast<UEdGraphNode_Dialogue>(EdNode_GraphNode)) {
			if (UEdGraphNode_Dialogue_Basic* EdNode_Dialogue_Baisc = Cast<UEdGraphNode_Dialogue_Basic>(EdNode_Dialogue)) { return SNew(SEdNode_Dialogue_Basic, EdNode_Dialogue_Baisc); }
			return SNew(SEdNode_DialogueNode, EdNode_Dialogue);
		}
		return SNew(SEdNode_DialogueNode, EdNode_GraphNode);
	}
	else if (UEdGraphNode_DialogueEdge* EdNode_Edge = Cast<UEdGraphNode_DialogueEdge>(Node))
	{
		return SNew(SEdNode_DialogueEdge, EdNode_Edge);
	}
	return nullptr;
}
