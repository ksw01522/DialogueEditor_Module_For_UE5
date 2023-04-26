// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_DialogueSession.h"
#include "DialogueAssetEditor/EdSchema_Dialogue.h"
#include "DialogueSession.h"
#include "DialogueAssetEditor/AssetEditor_Dialogue.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_Dialogue"

FAssetTypeActions_DialogueSession::FAssetTypeActions_DialogueSession(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
	
}

FText FAssetTypeActions_DialogueSession::GetName() const
{
	return LOCTEXT("FDialogueAssetTypeActionsName", "Dialogue Session");
}

FColor FAssetTypeActions_DialogueSession::GetTypeColor() const
{
	return FColor(129, 196, 115);
}

UClass* FAssetTypeActions_DialogueSession::GetSupportedClass() const
{
	return UDialogueSession::StaticClass();
}

void FAssetTypeActions_DialogueSession::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UDialogueSession* Graph = Cast<UDialogueSession>(*ObjIt))
		{
			TSharedRef<FAssetEditor_Dialogue> NewGraphEditor(new FAssetEditor_Dialogue());
			NewGraphEditor->InitDialogueAssetEditor(Mode, EditWithinLevelEditor, Graph);
		}
	}
}

uint32 FAssetTypeActions_DialogueSession::GetCategories()
{
	return MyAssetCategory;
}

#undef LOCTEXT_NAMESPACE