// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueNode.h"
#include "DialogueNode_End.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUERUNTIME_API UDialogueNode_End : public UDialogueNode
{
	GENERATED_BODY()
	
public:
	UDialogueNode_End();
#if WITH_EDITOR
	virtual FText GetNodeTitle() override;
#endif
};
