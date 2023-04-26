// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "DialogueEventFactory.generated.h"

/**
 * 
 */
UCLASS()
class UDialogueEventFactory : public UFactory
{
	GENERATED_BODY()
public:
	UDialogueEventFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;

};
