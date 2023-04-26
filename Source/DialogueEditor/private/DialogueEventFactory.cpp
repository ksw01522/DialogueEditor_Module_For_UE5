// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueEventFactory.h"
#include "Events/DialogueEvent.h"
#include "Kismet2/KismetEditorUtilities.h"


#define LOCTEXT_NAMESPACE "DialogueEvent"

UDialogueEventFactory::UDialogueEventFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDialogueEvent::StaticClass();
}

UObject* UDialogueEventFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (SupportedClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(SupportedClass))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), SupportedClass ? FText::FromString(SupportedClass->GetName()) : NSLOCTEXT("UnrealEd", "Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), Args));
		return nullptr;
	}

	return FKismetEditorUtilities::CreateBlueprint(
		SupportedClass,
		InParent,
		Name,
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass(),
		NAME_None
	);
}

FText UDialogueEventFactory::GetDisplayName() const
{
	return LOCTEXT("Dialogue Event Factory Name", "Dialogue Event");
}

#undef LOCTEXT_NAMESPACE