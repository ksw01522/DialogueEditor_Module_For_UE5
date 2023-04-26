// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueNode.h"
#include "DialogueNode_Start.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUERUNTIME_API UDialogueNode_Start : public UDialogueNode
{
	GENERATED_BODY()
	
public:
	UDialogueNode_Start();
#if WITH_EDITOR
	virtual FText GetNodeTitle() override;
	#endif
};
