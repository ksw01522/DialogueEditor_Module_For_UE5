// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DialogueStructs.h"
#include "DialogueManager.generated.h"

/**
 * 
 */
 class UDialoguerComponent;
 class UDialogueNode;
 class UDialogueNode_Basic;
 class UDialogueSession;
 class UDialogueEvent;


USTRUCT(BlueprintType)
struct FActingDialogueHandle
{
	GENERATED_BODY()

	friend UDialogueManager;

	FActingDialogueHandle() {}
	FActingDialogueHandle(uint32 ID) : ID(ID) {}

private:
	void Invalidate() { ID = INDEX_NONE; }

public:
	bool IsValid() const { return ID != INDEX_NONE; }

	bool operator==(const FActingDialogueHandle& Other) const { return ID == Other.ID; }
	bool operator!=(const FActingDialogueHandle& Other) const { return ID != Other.ID; }


	friend uint32 GetTypeHash(const FActingDialogueHandle& InHandle)
	{
		return InHandle.ID;
	}

private:
	uint32 ID = INDEX_NONE;
 };

USTRUCT(BlueprintType)
struct DIALOGUERUNTIME_API FActingDialogueData
{
	GENERATED_BODY()

	friend UDialogueManager;

	FActingDialogueData() : DialogueSession(nullptr), CurrentNode(nullptr) { Dialoguers.Empty(); DialogueEvents.Empty(); }
	FActingDialogueData(TArray<UDialoguerComponent*> NewDialoguers, UDialogueSession* Session);
	~FActingDialogueData() {}

private:
	TArray<TWeakObjectPtr<UDialoguerComponent>> Dialoguers;
	TWeakObjectPtr<UDialogueSession> DialogueSession;
	TWeakObjectPtr<UDialogueNode> CurrentNode;
	TArray<UDialogueEvent*> DialogueEvents;
private:

public:
	bool IsValidDialogue() const;
};
 


UCLASS(BlueprintType)
class DIALOGUERUNTIME_API UDialogueManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	UDialogueManager() {}

private:

	uint32 GDialogueHandleID = 0;

	TMap<FActingDialogueHandle, FActingDialogueData> ActingDialogueMap;
	EDialogueLanguage CurrentLanguage = EDialogueLanguage::Korean;

private:

	void RemoveDialogue(FActingDialogueHandle Target);

	bool GetElementFromNode(TArray<FDialogueElement>& OutElements, UDialogueNode_Basic* BasicNode) const;
	void GetElementsFromData(TArray<FDialogueElement>& OutElements, FActingDialogueData* Data) const;

	FActingDialogueData* GetActingDialogueData(FActingDialogueHandle& Handle);



public:
	UFUNCTION(BlueprintCallable, Category = "DialogueManager")
	FActingDialogueHandle EnterDialogue(TArray<UDialoguerComponent*> Dialoguers, UDialogueSession* Session);

	UFUNCTION(BlueprintCallable, Category = "DialogueManager")
	EDialogueNodeType EnterNextNode(TArray<FDialogueElement>& OutElements, UPARAM(ref) FActingDialogueHandle& Handle);

	void SetCurrentLanguage(EDialogueLanguage Lan) { CurrentLanguage = Lan; }

	UFUNCTION(BlueprintCallable, Category = "DialoguerManager")
	bool GetDialoguersInDialog(TArray<UDialoguerComponent*>& OutDialoguers, FActingDialogueHandle& Handle);

	bool CheckDialogueFromHandle(FActingDialogueHandle& Handle);

	/////////////////Events
private:
	void RemoveAllEvents(FActingDialogueHandle& Handle);
	void RemoveAllEvents(FActingDialogueData* Data);

	void CallEndEvents(FActingDialogueData* Data, bool bIsCancelled);

	bool IsCanEnterNextNode(FActingDialogueHandle& Handle);

public:
	bool AddDialogueEvent(UDialogueEvent* NewEvent, FActingDialogueHandle& Handle);
	bool RemoveDialogueEvent(UDialogueEvent* RemoveEvent, FActingDialogueHandle& Handle);
};
