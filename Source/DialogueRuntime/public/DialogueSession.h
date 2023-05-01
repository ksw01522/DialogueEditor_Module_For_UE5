// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "DialogueStructs.h"
#include "GameplayTagContainer.h"
#include "DialogueSession.generated.h"

/**
 * 
 */
 class UDialogueNode_Start;
 class UDialogueNode;
 class UDataTable;

 UENUM(BlueprintType)
	 enum class ECustomNodeTypeHidden : uint8
 {
	 Visible,
	 Hidden
 };


UCLASS()
class DIALOGUERUNTIME_API UDialogueSession : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY()
	ECustomNodeTypeHidden bHideNodeType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FGameplayTagContainer DialogueTags;

	UPROPERTY()
	UDialogueNode_Start* StartNode = nullptr;

	UPROPERTY()
	TArray<UDialogueNode*> AllNodes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	bool bEdgeEnabled;

private:
	UPROPERTY(EditAnywhere, Category = "DialogueTextStyle", meta = (RequiredAssetDataTags = "RowStructure=/Script/UMG.RichTextStyleRow"))
	UDataTable* DialogueTextStyleSet;

	UPROPERTY(EditAnywhere, Category = "DialogueTextStyle")
	TArray<TSubclassOf<URichTextBlockDecorator>> DecoratorClasses;

public:
#if WITH_EDITOR
	//UFUNCTION(BlueprintCallable, Category = "Dialogue")
	//virtual void Print(bool ToConsole = true, bool ToScreen = true);
#endif

	void ClearGraph();

#if WITH_EDITORONLY_DATA
	UPROPERTY()
		class UEdGraph* EdGraph;

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue_Editor")
		bool bCanRenameNode;

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue_Editor")
		bool bCanBeCyclical;
#endif

public:
	UDialogueSession();
	virtual ~UDialogueSession() {}

public:


#if WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	UDataTable* DialoguerNameTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	UDataTable* DialogueStringTable;

	UPROPERTY(EditAnywhere, Category = "DialogueEditor")
	EDialogueLanguage PreviewLanguage;
#endif

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue")
	UDialogueNode_Start* GetStartNode() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue")
	UDataTable* GetDialogueTextStyleSet() const {return DialogueTextStyleSet;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue")
	TArray<TSubclassOf<class URichTextBlockDecorator>> GetDialogueDecoClasses() const {return DecoratorClasses;}

#if WITH_EDITOR
	
public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	void OnChaged_PreviewLanguage();

#endif
};
