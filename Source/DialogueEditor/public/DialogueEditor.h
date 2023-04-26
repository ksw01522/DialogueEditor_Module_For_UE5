// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogueEditorModule.h"
#include <IAssetTools.h>
#include <EdGraphUtilities.h>

#define CUSTOMLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define LOG_INFO(FMT, ...) UE_LOG(DialogueEditor, Display, TEXT("%s %s"), *CUSTOMLOG_CALLINFO, *FString::Printf(FMT, ##__VA_ARGS__) )
#define LOG_WARNING(FMT, ...) UE_LOG(DialogueEditor, Warning, TEXT("%s %s"), *CUSTOMLOG_CALLINFO, *FString::Printf(FMT, ##__VA_ARGS__) )
#define LOG_ERROR(FMT, ...) UE_LOG(DialogueEditor, Error, TEXT("%s %s"), *CUSTOMLOG_CALLINFO, *FString::Printf(FMT, ##__VA_ARGS__) )


class FDialogueEditor : public IDialogueEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


private:
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);

	void RegisterCategory();

	void RegisterNodeFactory();
	void UnRegisterNodeFactory();

	void RegisterBlueprintEvents();

	void HandleNewDialogueEventBlueprintCreated(UBlueprint* Blueprint);


private:
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;

	TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_Dialogue;

	EAssetTypeCategories::Type DialogueAssetCategoryBit;
};

