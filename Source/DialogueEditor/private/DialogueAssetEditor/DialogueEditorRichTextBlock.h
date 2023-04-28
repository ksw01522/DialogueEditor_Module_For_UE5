// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "DialogueEditorRichTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class UDialogueEditorRichTextBlock : public URichTextBlock
{
	GENERATED_BODY()

public:
	const TSharedPtr<class FSlateStyleSet>& GetDialogueTextStyleInstance() const;
	const TArray<TObjectPtr<URichTextBlockDecorator>>& GetDialogueTextBlockDecoratorinstances() const;
};
