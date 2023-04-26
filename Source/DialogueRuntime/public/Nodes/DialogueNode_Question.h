// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueNode_Basic.h"
#include "DialogueNode_Answer.h"
#include "DialogueNode_Question.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUERUNTIME_API UDialogueNode_Question : public UDialogueNode_Basic
{
	GENERATED_BODY()
public:
	UDialogueNode_Question();

public:
	//virtual bool CanCreateConnection(UDialogueNode* Other, FText& ErrorMessage) override;
#if WITH_EDITOR
	virtual bool CanCreateConnectionTo(UDialogueNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage) override;

	virtual FText GetNodeTitle() override;
	#endif
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	TArray<UDialogueNode_Answer*> GetAnswers();
};
