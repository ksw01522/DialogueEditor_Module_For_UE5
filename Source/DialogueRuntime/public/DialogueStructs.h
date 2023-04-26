// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CompositeDataTable.h"
#include "DialogueStructs.generated.h"


UENUM(BlueprintType)
enum class EDialogueLanguage : uint8
{
	Korean		UMETA(DisplayName = "Korean"),
	English		UMETA(DisplayName = "English")
};

UENUM(BlueprintType)
enum class EDialogueNodeType : uint8
{
	None = 0		,
	Start			UMETA(DisplayName = "Start"),
	Basic			UMETA(DisplayName = "Basic"),
	Question		UMETA(DisplayName = "Question"),
	Answer			UMETA(DisplayName = "Answer"),
	End				UMETA(DislplayName = "End")
};

USTRUCT(BlueprintType, Atomic)
struct FDialogueElement
{
	GENERATED_BODY()

	FDialogueElement() {}


public:
	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString DialogString;
};

USTRUCT(Atomic)
struct FDialogueLocalization : public FTableRowBase
{
	GENERATED_BODY()

public:
	FDialogueLocalization() : Original(), Korean(), English()
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Original;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Korean;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString English;

	void DialogueClear() {
		Original = "";
		Korean = "";
		English = "";
	}
};

