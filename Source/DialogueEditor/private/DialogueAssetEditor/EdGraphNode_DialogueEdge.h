// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "DialogueEdge.h"
#include "EdGraphNode_DialogueEdge.generated.h"

class UDialogueNode;
class UEdGraphNode_Dialogue;

/**
 * 
 */
UCLASS(MinimalAPI)
class UEdGraphNode_DialogueEdge : public UEdGraphNode
{
	GENERATED_BODY()
	
public:
	UEdGraphNode_DialogueEdge();

	UPROPERTY()
	class UEdGraph* Graph;

	UPROPERTY(VisibleAnywhere, Instanced, Category = "Dialogue")
	UDialogueEdge* DialogueEdge;

	void SetEdge(UDialogueEdge* Edge);

	virtual void AllocateDefaultPins();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual void PrepareForCopying() override;

	virtual UEdGraphPin* GetInputPin() const {return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const {return Pins[1]; }

	void CreateConnections(UEdGraphNode_Dialogue* Start, UEdGraphNode_Dialogue* End);

	UEdGraphNode_Dialogue* GetStartNode();
	UEdGraphNode_Dialogue* GetEndNode();
};
