// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogueNode_Basic.h"
#include "DialogueNode_Answer.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUERUNTIME_API UDialogueNode_Answer : public UDialogueNode_Basic
{
	GENERATED_BODY()
public:
	UDialogueNode_Answer();

public:
	//virtual bool CanCreateConnection(UDialogueNode* Other, FText& ErrorMessage) override;

#if WITH_EDITOR
	virtual bool CanCreateConnectionFrom(UDialogueNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage) override;

	virtual FText GetNodeTitle() override;
#endif
};
