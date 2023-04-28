// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/DialogueEditorRichTextBlock.h"

const TSharedPtr<class FSlateStyleSet>& UDialogueEditorRichTextBlock::GetDialogueTextStyleInstance() const
{
	return StyleInstance;
}

const TArray<TObjectPtr<URichTextBlockDecorator>>& UDialogueEditorRichTextBlock::GetDialogueTextBlockDecoratorinstances() const
{
	return InstanceDecorators;
}
 