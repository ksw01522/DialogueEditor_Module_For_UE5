// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueManager.h"
#include "DialoguerComponent.generated.h"




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIALOGUERUNTIME_API UDialoguerComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UDialogueManager;

public:	
	// Sets default values for this component's properties
	UDialoguerComponent();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialoguer", meta = (AllowPrivateAccess = "true"))
	FString DialoguerID;

	FActingDialogueHandle DialogueHandle;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:		
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void OnEnteredDialogue(const FActingDialogueHandle& Handle);

public:

	FString GetDialoguerID() const {return DialoguerID;}
	
	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue")
	FActingDialogueHandle GetDialogueHandle() { return DialogueHandle; };
};
