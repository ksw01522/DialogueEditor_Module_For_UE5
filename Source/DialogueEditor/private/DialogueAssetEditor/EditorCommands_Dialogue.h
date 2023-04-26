// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
/**
 * 
 */
class FEditorCommands_Dialogue : public TCommands<FEditorCommands_Dialogue>
{
public:
	FEditorCommands_Dialogue()
		:	TCommands<FEditorCommands_Dialogue>("Dialogue Editor", NSLOCTEXT("Contexts", "Dialogue Editor", "Dialogue Editor"), NAME_None, FAppStyle::GetAppStyleSetName())
	{}

	TSharedPtr<FUICommandInfo> DialogueSettings;
	TSharedPtr<FUICommandInfo> AutoArrange;

	virtual void RegisterCommands() override;
};
