// Copyright Epic Games, Inc. All Rights Reserved.

#include "DialogueEditor.h"
#include "DialogueNodeFactory.h"
#include "AssetTypeActions_DialogueSession.h"
#include "DialogueAssetEditor/DialogueEditorStyle.h"
#include "AssetTypeActions_DialogueSession.h"
#include "AssetTypeActions_DialogueEvent.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "K2Node_Event.h"

#include "Events/DialogueEvent.h"
#include "FDialogueEditorUtilities.h"



DEFINE_LOG_CATEGORY(DialogueEditor)
IMPLEMENT_MODULE(FDialogueEditor, DialogueEditor)

#define LOCTEXT_NAMESPACE "Editor_Dialogue"

void FDialogueEditor::StartupModule()
{
	FDialogueEditorStyle::Initialize();

	RegisterNodeFactory();
	RegisterCategory();

	RegisterBlueprintEvents();
}

void FDialogueEditor::ShutdownModule()
{	
// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
		}
	}

	UnRegisterNodeFactory();

	FDialogueEditorStyle::Shutdown();
}


void FDialogueEditor::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

void FDialogueEditor::RegisterCategory()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	DialogueAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("DialogueSession")), LOCTEXT("DialogueAssetCategory", "DialogueSession"));

	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_DialogueSession(DialogueAssetCategoryBit)));
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_DialogueEvent(DialogueAssetCategoryBit)));
}

void FDialogueEditor::RegisterNodeFactory()
{
	GraphPanelNodeFactory_Dialogue = MakeShareable(new FGraphPanelNodeFactory_Dialogue());
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_Dialogue);
}

void FDialogueEditor::UnRegisterNodeFactory()
{
	if (GraphPanelNodeFactory_Dialogue.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_Dialogue);
		GraphPanelNodeFactory_Dialogue.Reset();
	}
}

void FDialogueEditor::RegisterBlueprintEvents()
{
	FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback(
		this,
		UDialogueEvent::StaticClass(),
		FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(this, &FDialogueEditor::HandleNewDialogueEventBlueprintCreated)
	);

}

void FDialogueEditor::HandleNewDialogueEventBlueprintCreated(UBlueprint* Blueprint)
{

	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return;
	}

	Blueprint->bForceFullEditor = true;
	UK2Node_Event* EventNode = FDialogueEditorUtilities::BlueprintGetOrAddEvent(
		Blueprint,
		GET_FUNCTION_NAME_CHECKED(UDialogueEvent, CallEvent),
		UDialogueEvent::StaticClass()
	);
	if (EventNode)
	{
		Blueprint->LastEditedDocuments.Add(EventNode->GetGraph());
	}
	
}


#undef LOCTEXT_NAMESPACE
