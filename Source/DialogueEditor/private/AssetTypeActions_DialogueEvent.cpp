// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_DialogueEvent.h"
#include "Events/DialogueEvent.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_Dialogue"

FText FAssetTypeActions_DialogueEvent::GetName() const
{
	return LOCTEXT("FDialogueEventAssetTypeActionsName", "Dialogue Custom Event");
}

UClass* FAssetTypeActions_DialogueEvent::GetSupportedClass() const
{
	return UDialogueEvent::StaticClass();
}



#undef LOCTEXT_NAMESPACE