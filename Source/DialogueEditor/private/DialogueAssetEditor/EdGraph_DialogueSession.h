// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph_DialogueSession.generated.h"

class UDialogueSession;
class UDialogueNode;
class UDialogueEdge;
class UEdGraphNode_Dialogue;
class UEdGraphNode_DialogueEdge;

UCLASS()
class UEdGraph_DialogueSession : public UEdGraph
{
	GENERATED_BODY()
	
public:
	UEdGraph_DialogueSession();
	virtual ~UEdGraph_DialogueSession();

	virtual void RebuildDialogueSession();

	UDialogueSession* GetDialogueSession() const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;

	UPROPERTY(Transient)
	TMap<UDialogueNode*, UEdGraphNode_Dialogue*> NodeMap;

	UPROPERTY(Transient)
	TMap<UDialogueEdge*, UEdGraphNode_DialogueEdge*> EdgeMap;

	virtual void PostLoad() override;

protected:
	void Clear();

	void SortNode(UDialogueNode* TargetNode);

};
