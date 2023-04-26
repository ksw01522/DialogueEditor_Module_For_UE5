// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/EditorCommands_Dialogue.h"

#define LOCTEXT_NAMESPACE "EditorCommands_Dialogue"

void FEditorCommands_Dialogue::RegisterCommands()
{
	UI_COMMAND(DialogueSettings, "Dialogue Settings", "Dialogue Settings", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AutoArrange, "Auto Arrange", "Auto Arrange", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
