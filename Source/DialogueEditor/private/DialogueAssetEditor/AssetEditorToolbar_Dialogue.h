// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FAssetEditor_Dialogue;
class FExtender;
class FToolBarBuilder;

/**
 * 
 */
class FAssetEditorToolbar_Dialogue : public TSharedFromThis<FAssetEditorToolbar_Dialogue>
{
public:
	FAssetEditorToolbar_Dialogue(TSharedPtr<FAssetEditor_Dialogue> InDialogueEditor)
		: DialogueEditor(InDialogueEditor) {}

	void AddDialogueToolbar(TSharedPtr<FExtender> Extender);

private:
	void FillDialogueToolbar(FToolBarBuilder& ToolbarBuilder);

protected:
	/** Pointer back to the blueprint editor tool that owns us */
	TWeakPtr<FAssetEditor_Dialogue> DialogueEditor;
};
