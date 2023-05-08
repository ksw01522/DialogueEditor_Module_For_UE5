// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogueNode_Basic.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
#include "EdGraphNode_Dialogue_Basic.generated.h"

/**
 * 
 */
 
UCLASS()
class UEdGraphNode_Dialogue_Basic : public UEdGraphNode_Dialogue
{
	GENERATED_BODY()

public:
	class SEdNode_Dialogue_Basic* SEdNode_Basic;

	UDialogueNode_Basic* GetDialogueBasicNode() const;

	UDataTable* GetDialogueTextStyleSet() const;
	TArray<TSubclassOf<class URichTextBlockDecorator>> GetUMGDecoClasses() const;
	TArray<TSubclassOf<class USRichTextBlockDecorator>> GetSlateDecoClasses() const;
	ERichTextBlockType GetTextBlockType() const;


	FString GetDialoguerName() const;
	FString GetDialoguerName_Original() const;

	FString GetDialogueString() const;
	FString GetDialogueString_Original() const;

	void TryImportDialogueString() const;

	UFUNCTION()
	void OnChangedDialogueStyleFunction();

	virtual void SetDialogueNode(UDialogueNode* NewNode) override;
};
