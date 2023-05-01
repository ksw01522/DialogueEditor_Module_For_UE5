// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/DialogueNode_Basic.h"
#include "DialogueRuntime.h"
#include "DialogueSession.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DialogueStructs.h"


#define LOCTEXT_NAMESPACE "DialogueNode"

UDialogueNode_Basic::UDialogueNode_Basic()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("Dialogue Node Basic Title", "Basic Dialogue");
	ChildrenLimitType = ECustomNodeLimit::Limited;
	ChildrenLimit = 1;
#endif
	DialogueTextStyleSet = nullptr;
	DialogueNodeType = EDialogueNodeType::Basic;
}

UDataTable* UDialogueNode_Basic::GetDialogueTextStyleSet() const
{
	if (DialogueTextStyleSet != nullptr)		{ return DialogueTextStyleSet; }
	else										{ return DialogueSession->GetDialogueTextStyleSet(); }
}

TArray<TSubclassOf<class URichTextBlockDecorator>> UDialogueNode_Basic::GetDecoClasses() const
{
	if (DialogueDecoratorClasses.IsEmpty())		{ return GetDialogueSession()->GetDialogueDecoClasses();} 
	else										{ return DialogueDecoratorClasses;}
}

FString UDialogueNode_Basic::GetDialoguerName(EDialogueLanguage Language)
{
	FString ReturnString = "Narration";

#if WITH_EDITOR
	OnChanged_DialoguerNameCode();
#endif

	if(Dialogue_Name.Original == "") return ReturnString;

	switch (Language) {
		case EDialogueLanguage::Korean:
			ReturnString = Dialogue_Name.Korean;
		break;

		case EDialogueLanguage::English:
			ReturnString = Dialogue_Name.English;
		break;
	}

	return ReturnString;
}

FString UDialogueNode_Basic::GetDialogueString(EDialogueLanguage Language)
{
	FString ReturnString;

#if WITH_EDITOR
	OnChanged_DialogueStringCode();
#endif

	switch (Language) {
	case EDialogueLanguage::Korean:
		ReturnString = Dialogue_String.Korean;
		break;

	case EDialogueLanguage::English:
		ReturnString = Dialogue_String.English;
		break;
	}

	return ReturnString;
}




#if WITH_EDITOR

FString UDialogueNode_Basic::GetDialoguerName_InEditor() const
{
	if (DialoguerNameCode.IsNone()) {
		return FString("Narration");
	}

	return DialoguerName;
}



FString UDialogueNode_Basic::GetDialogueString_InEditor() const
{
	return DialogueString;
}


FString UDialogueNode_Basic::GetDialoguerName_Original_InEditor() const
{
	if (DialoguerNameCode.IsNone()) {
		return FString("Narration");
	}
	return Original_DialoguerName;
}

FString UDialogueNode_Basic::GetDialogueString_Original_InEditor() const
{
	return Original_DialogueString;
}

FText UDialogueNode_Basic::GetNodeTitle()
{
	//return DialogueString.IsEmpty() ? LOCTEXT( "Empty Node Title", "Empty String") : FText::FromString(DialogueString);
	return FText::FromString(GetDialoguerName_InEditor());
}
FText UDialogueNode_Basic::GetDescription_Implementation() const
{
	return FText::FromString(GetDialogueString_InEditor());
}
void UDialogueNode_Basic::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if		(PropertyChangedEvent.GetPropertyName() == "DialoguerNameCode")		{ OnChanged_DialoguerNameCode(); }
	else if (PropertyChangedEvent.GetPropertyName() == "DialogueStringCode")	{ OnChanged_DialogueStringCode(); }
	else if (PropertyChangedEvent.GetPropertyName() == "DialogueTextStyleSet" || PropertyChangedEvent.GetPropertyName() == "DialogueDecoratorClasses") { OnChangedDialogueTextStyle(); }

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

FDialogueLocalization* UDialogueNode_Basic::GetDialoguerNameRow()
{
	if (!IsValid(DialogueSession)) {
		LOG_ERROR(TEXT("Can't Find Dialogue Session."));
		return nullptr;
	}

	UDataTable* NameTable = DialogueSession->DialoguerNameTable;
	if (NameTable == nullptr) {
		FString SessionName;
		DialogueSession->GetName(SessionName);
		LOG_WARNING(TEXT("Can't Find DialoguerNameTable From %s"), *SessionName);
		return nullptr;
	}

	FString TempContext("");
	FDialogueLocalization* NameRow = NameTable->FindRow<FDialogueLocalization>(DialoguerNameCode, TempContext);
	if (NameRow == nullptr) {
		LOG_WARNING(TEXT("Can't Find DialoguerName by code"));
		return nullptr;
	}

	return NameRow;
}

FDialogueLocalization* UDialogueNode_Basic::GetDialogueStringRow()
{
	if (!IsValid(DialogueSession)) {
		LOG_ERROR(TEXT("Can't Find Dialogue Session."));
		return nullptr;
	}
	
	if (DialogueStringCode.IsNone()) {
		LOG_WARNING(TEXT("DialogueStringCode is None. Plz put Code"));
		return nullptr;
	}

	UDataTable* DialogueTable = DialogueSession->DialogueStringTable;
	if (DialogueTable == nullptr) {
		FString SessionName;
		DialogueSession->GetName(SessionName);
		LOG_WARNING(TEXT("Can't Find DialogueStringTable From %s"), *SessionName);
		return nullptr;
	}

	FString TempContext("");
	FDialogueLocalization* DialogueRow = DialogueTable->FindRow<FDialogueLocalization>(DialogueStringCode, TempContext);
	if (DialogueRow == nullptr) {
		LOG_WARNING(TEXT("Can't Find Dialogue string by code"));
		return nullptr;
	}

	return DialogueRow;
}



void UDialogueNode_Basic::ImportDialogue()
{
	if (!IsValid(DialogueSession)) {
		LOG_ERROR(TEXT("Can't Find Dialogue Session."));
		return;
	}

	ImportDialogue_String();
}

void UDialogueNode_Basic::RebuildNode()
{
	OnChanged_DialoguerNameCode();
	OnChanged_DialogueStringCode();
}

void UDialogueNode_Basic::ImportDialogue_String()
{
	if(!CanVisible_DialogueString) {return;}

	FDialogueLocalization* DialogueRow = GetDialogueStringRow();
	if (DialogueRow == nullptr) { 
		DialogueSession->DialogueStringTable->AddRow(DialogueStringCode, FDialogueLocalization());
		DialogueRow = GetDialogueStringRow();
	}

	if(DialogueRow->Original != Original_DialogueString) { DialogueRow->Original = Original_DialogueString; }
	switch (DialogueSession->PreviewLanguage)
	{
	case EDialogueLanguage::Korean:
		if (DialogueRow->Korean != DialogueString) {
			DialogueRow->Korean = DialogueString;
		}
		break;

	case EDialogueLanguage::English:
		if (DialogueRow->English != DialogueString) {
			DialogueRow->English = DialogueString;
		}
		break;

	default:
		break;
	}

	Dialogue_String = *DialogueRow;
}

void UDialogueNode_Basic::OnChanged_DialoguerNameCode()
{
	FDialogueLocalization* NameRow = GetDialoguerNameRow();
	if(NameRow == nullptr){ 
		CanVisible_DialoguerName = false;
		Dialogue_Name.DialogueClear();
		return; 
	}
	Dialogue_Name = *NameRow;

	CanVisible_DialoguerName = true;
	Original_DialoguerName = NameRow->Original;
	switch (DialogueSession->PreviewLanguage)
	{
		case EDialogueLanguage::Korean:
			DialoguerName = NameRow->Korean;
			break;
		
		case EDialogueLanguage::English:
			DialoguerName =NameRow->English;
			break;

		default:
			break;
	}
}

void UDialogueNode_Basic::OnChanged_DialogueStringCode()
{
	LOG_INFO(TEXT("Changed String Code"));

	FDialogueLocalization* DialogueRow = GetDialogueStringRow();
	if(DialogueSession == nullptr || DialogueSession->DialogueStringTable == nullptr || DialogueRow == nullptr) { 
		CanVisible_DialogueString = false;
		Dialogue_String.DialogueClear();
		return; 
	}
	Dialogue_String = *DialogueRow;

	CanVisible_DialogueString = true;

	if (DialogueRow == nullptr)
	{
		Original_DialogueString = "";
		DialogueString = "";
	}
	else {
		Original_DialogueString = DialogueRow->Original;
		switch (DialogueSession->PreviewLanguage)
		{
		case EDialogueLanguage::Korean:
			DialogueString = DialogueRow->Korean;
			break;

		case EDialogueLanguage::English:
			DialogueString = DialogueRow->English;
			break;

		default:
			break;
		}
	}
}

void UDialogueNode_Basic::OnChanged_PreviewLanguage()
{
	if (!IsValid(DialogueSession)) {
		LOG_ERROR(TEXT("Can't Find Dialogue Session."));
		return;
	}

	OnChanged_DialoguerNameCode();
	OnChanged_DialogueStringCode();
}
void UDialogueNode_Basic::OnChangedDialogueTextStyle()
{
	OnChangedDialogueStyle.ExecuteIfBound();
}
#endif

#undef LOCTEXT_NAMESPACE

