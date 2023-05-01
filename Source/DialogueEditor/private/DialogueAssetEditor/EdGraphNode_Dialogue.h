// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphNode_Dialogue.generated.h"

/**
 * 
 */
 class SEdNode_DialogueNode;
 class UEdGraph_DialogueSession;
 class UDialogueSession;
 class UDialogueNode;

UCLASS()
class UEdGraphNode_Dialogue : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UEdGraphNode_Dialogue();
	virtual ~UEdGraphNode_Dialogue(){};

	UDialogueNode* GetDialogueNode() const;

	UPROPERTY(VisibleAnywhere, Instanced, Category = "DialogueSession")
	UDialogueNode* DialogueNode;

	virtual void SetDialogueNode(UDialogueNode* InNode);
	UEdGraph_DialogueSession* GetDialogueEdGraph();

	SEdNode_DialogueNode* SEdNode;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PrepareForCopying() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual FLinearColor GetBackgroundColor() const;
	virtual UEdGraphPin* GetInputPin() const;
	virtual UEdGraphPin* GetOutputPin() const;

	virtual void PostEditUndo() override;

	FText GetNodeDescription();



};
