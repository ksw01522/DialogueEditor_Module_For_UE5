// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "DialogueStructs.h"
#include "DialogueManager.h"
#include "DialogueNode.generated.h"

/**
 * 
 */



class UDialogueEvent;
class APlayerController;
class UDialogueSession;

UENUM(BlueprintType)
enum class ECustomNodeLimit : uint8
{
	Unlimited,
	Limited
};


UCLASS(Abstract, BlueprintType)
class DIALOGUERUNTIME_API UDialogueNode : public UObject
{
	GENERATED_BODY()

public:
	UDialogueNode();
	virtual ~UDialogueNode() {}

public:
	UPROPERTY()
	UDialogueSession* DialogueSession;

	UPROPERTY(BlueprintReadOnly, Category = "DialogueNode")
	TArray<UDialogueNode*> ParentNodes;

	UPROPERTY(BlueprintReadOnly, Category = "DialogueNode")
	TArray<UDialogueNode*> ChildrenNodes;

	UPROPERTY(BlueprintReadOnly, Category = "DialogueNode")
	TMap<UDialogueNode*, UDialogueEdge*> Edges;

	UPROPERTY(VisibleAnywhere)
	EDialogueNodeType DialogueNodeType;

private:
	UPROPERTY(EditAnywhere, Category = "DialogueEvents", Instanced, meta = (AllowPrivateAccess = "true"))
	TArray<UDialogueEvent*> EnterEvents;


public:
	UFUNCTION(BlueprintCallable, Category = "DialogueNode")
	virtual UDialogueEdge* GetEdge(UDialogueNode* ChildNode);


	UFUNCTION(BlueprintCallable, Category = "DialogueNode")
	bool IsLeafNode() const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "DialogueNode")
	UDialogueSession* GetDialogueSession() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DialogueNode")
	EDialogueNodeType GetDialogueNodeType() const { return DialogueNodeType; }

	UFUNCTION(BlueprintCallable, Category = "DialogueNode")
	void CallEvents(FActingDialogueHandle& Handle);

#if WITH_EDITOR
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DialogueNode")
	FText GetDescription() const;
	virtual FText GetDescription_Implementation() const;


	virtual void OnChangedDialogueTextStyle() {}

#endif

#if WITH_EDITORONLY_DATA
	
	UPROPERTY(EditDefaultsOnly, Category = "DialogueNode_Editor")
	FLinearColor BackgroundColor;

	//노드 생성 이름
	UPROPERTY(EditDefaultsOnly, Category = "DialogueNode_Editor")
	FText ContextMenuName;

	UPROPERTY(EditDefaultsOnly, Category = "DialogueNode_Editor")
	ECustomNodeLimit ParentLimitType;

	UPROPERTY(EditDefaultsOnly, Category = "DialogueNode_Editor", meta = (ClampMin = "0", EditCondition = "ParentLimitType == ECustomNodeLimit::Limited", EditConditionHides))
	int32 ParentLimit;

	UPROPERTY(EditDefaultsOnly, Category = "DialogueNode_Editor")
	ECustomNodeLimit ChildrenLimitType;

	UPROPERTY(EditDefaultsOnly, Category = "DialogueNode_Editor", meta = (ClampMin = "0", EditCondition = "ChildrenLimitType == ECustomNodeLimit::Limited", EditConditionHides))
	int32 ChildrenLimit;


#endif

#if WITH_EDITOR

	virtual bool IsNameEditable() const;

	virtual FLinearColor GetBackgroundColor() const;

	virtual FText GetNodeTitle();

	virtual void SetNodeTitle(const FText& NewTitle) {};

	virtual bool CanCreateConnection(UDialogueNode* Other, FText& ErrorMessage);

	virtual bool CanCreateConnectionTo(UDialogueNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage);
	virtual bool CanCreateConnectionFrom(UDialogueNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage);

	virtual void RebuildNode() {};

public:
	virtual void OnChanged_PreviewLanguage() {};
#endif
};
