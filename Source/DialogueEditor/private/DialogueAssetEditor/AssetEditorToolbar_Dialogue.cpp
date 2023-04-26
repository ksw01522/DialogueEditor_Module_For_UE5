// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/AssetEditorToolbar_Dialogue.h"
#include "DialogueAssetEditor/AssetEditor_Dialogue.h"
#include "DialogueAssetEditor/EditorCommands_Dialogue.h"
#include "DialogueAssetEditor/DialogueEditorStyle.h"

#define LOCTEXT_NAMESPACE "AssetEditorToolbar_Dialogue"

void FAssetEditorToolbar_Dialogue::AddDialogueToolbar(TSharedPtr<FExtender> Extender)
{
	check(DialogueEditor.IsValid());
	TSharedPtr<FAssetEditor_Dialogue> DialogueEditorPtr = DialogueEditor.Pin();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, DialogueEditorPtr->GetToolkitCommands(), FToolBarExtensionDelegate::CreateSP(this, &FAssetEditorToolbar_Dialogue::FillDialogueToolbar));
	DialogueEditorPtr->AddToolbarExtender(ToolbarExtender);

}

void FAssetEditorToolbar_Dialogue::FillDialogueToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(DialogueEditor.IsValid());
	TSharedPtr<FAssetEditor_Dialogue> DialogueEditorPtr = DialogueEditor.Pin();

	ToolbarBuilder.BeginSection("Dialogue Editor");
	{
		ToolbarBuilder.AddToolBarButton(FEditorCommands_Dialogue::Get().DialogueSettings,
			NAME_None,
			LOCTEXT("DialogueSettings_Label", "Dialogue Settings"),
			LOCTEXT("DialogueSettings_ToolTip", "Show the Dialogue Settings"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.GameSettings"));
	}
	ToolbarBuilder.EndSection();

	ToolbarBuilder.BeginSection("Util");
	{
		ToolbarBuilder.AddToolBarButton(FEditorCommands_Dialogue::Get().AutoArrange,
			NAME_None,
			LOCTEXT("AutoArrange_Label", "Auto Arrange"),
			LOCTEXT("AutoArrange_ToolTip", "Auto arrange nodes, not perfect, but still handy"),
			FSlateIcon(FDialogueEditorStyle::GetStyleSetName(), "DialogueEditor.AutoArrange"));
	}
	ToolbarBuilder.EndSection();

}

#undef LOCTEXT_NAMESPACE