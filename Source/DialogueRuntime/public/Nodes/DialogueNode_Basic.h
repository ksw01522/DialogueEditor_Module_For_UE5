// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueNode.h"
#include "DialogueStructs.h"
#include "DialogueNode_Basic.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUERUNTIME_API UDialogueNode_Basic : public UDialogueNode
{
	GENERATED_BODY()

#if WITH_EDITOR
	DECLARE_DYNAMIC_DELEGATE(FOnChangedDialogueStyle);
#endif


protected:
	UPROPERTY()
	FDialogueLocalization Dialogue_Name;
	UPROPERTY()
	FDialogueLocalization Dialogue_String;



#if WITH_EDITORONLY_DATA

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Dialogue")
	FName DialoguerNameCode;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Dialogue")
	FName DialogueStringCode;

	UPROPERTY(VisibleDefaultsOnly, Category = "Dialogue", AdvancedDisplay)
	bool CanVisible_DialoguerName = false;

	UPROPERTY(VisibleDefaultsOnly, Category = "Dialogue", AdvancedDisplay)
	bool CanVisible_DialogueString = false;

	UPROPERTY(VisibleInstanceOnly,  Category = "Dialogue", BlueprintGetter = GetDialoguerName_Original_InEditor, meta = (DisplayAfter = "DialoguerNameCode", EditCondition = "CanVisible_DialoguerName", EditConditionHides))
	FString Original_DialoguerName;

	UPROPERTY(VisibleInstanceOnly,  Category = "Dialogue", BlueprintGetter = GetDialoguerName_InEditor, meta = (DisplayAfter = "DialoguerNameCode", EditCondition = "CanVisible_DialoguerName", EditConditionHides))
	FString DialoguerName;

	UPROPERTY(EditInstanceOnly, Category = "Dialogue", BlueprintGetter = GetDialogueString_Original_InEditor, meta = (DisplayAfter = "DialogueStringCode", EditCondition = "CanVisible_DialogueString", EditConditionHides))
	FString Original_DialogueString;

	UPROPERTY(EditInstanceOnly, Category = "Dialogue", BlueprintGetter = GetDialogueString_InEditor, meta = (DisplayAfter = "DialogueStringCode", EditCondition = "CanVisible_DialogueString", EditConditionHides))
	FString DialogueString;

	
#endif

	UPROPERTY(EditAnywhere, Category = "DialogueTextStyle", meta = (RequiredAssetDataTags = "RowStructure=/Script/UMG.RichTextStyleRow"))
	UDataTable* DialogueTextStyleSet;

	UPROPERTY(EditAnywhere, Category = "DialogueTextStyle")
	TArray<TSubclassOf<class URichTextBlockDecorator>> DialogueDecoratorClasses;

public:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	FOnChangedDialogueStyle OnChangedDialogueStyle;
#endif


public:
	UDialogueNode_Basic();

	UDataTable* GetDialogueTextStyleSet() const;
	TArray<TSubclassOf<class URichTextBlockDecorator>> GetDecoClasses() const;

	FString GetDialoguerName(EDialogueLanguage Language);
	FString GetDialogueString(EDialogueLanguage Language);

#if WITH_EDITOR

	UFUNCTION(BlueprintCallable, BlueprintGetter, Category = "Dialogue")
	FString GetDialoguerName_InEditor() const;

	UFUNCTION(BlueprintCallable, BlueprintGetter, Category = "Dialogue")
	FString GetDialogueString_InEditor() const;

	UFUNCTION(BlueprintCallable, BlueprintGetter, Category = "Dialogue")
	FString GetDialoguerName_Original_InEditor() const;

	UFUNCTION(BlueprintCallable, BlueprintGetter, Category = "Dialogue")
	FString GetDialogueString_Original_InEditor() const;

	virtual FText GetNodeTitle() override;

	virtual FText GetDescription_Implementation() const override;
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION()
	virtual void ImportDialogue();

	virtual void RebuildNode() override;

private:
	FDialogueLocalization* GetDialoguerNameRow();
	FDialogueLocalization* GetDialogueStringRow();


	virtual void ImportDialogue_String();

	virtual void OnChanged_DialoguerNameCode();
	virtual void OnChanged_DialogueStringCode();

	virtual void OnChanged_PreviewLanguage() override;

	virtual void OnChangedDialogueTextStyle() override;

#endif
	
};
