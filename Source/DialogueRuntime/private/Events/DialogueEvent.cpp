// Fill out your copyright notice in the Description page of Project Settings.


#include "Events/DialogueEvent.h"
#include "DialogueManager.h"
#include "DialogueRuntime.h"

void UDialogueEvent::CallEvent(FActingDialogueHandle& Handle)
{
	if (!Handle.IsValid()) {
		LOG_ERROR(TEXT("Dialogue Handle Is Not Valid."));
		return;
	}

	if (InstancingPolicy == EDialougeEventInstancingPolicy::NotInstanced) {
		UDialogueManager* Manager = UGameInstance::GetSubsystem<UDialogueManager>(GetWorld()->GetGameInstance());
		Manager->AddDialogueEvent(this, Handle);
		OnCalledEvent(Handle);
		CalledDialogueID = Handle;
	}
	else if (InstancingPolicy == EDialougeEventInstancingPolicy::InstancedPerExecution) {
		UDialogueEvent* InstancedObject = NewObject<UDialogueEvent>(nullptr, StaticClass(), NAME_None, EObjectFlags::RF_NoFlags, this);
		UDialogueManager* Manager = UGameInstance::GetSubsystem<UDialogueManager>(GetWorld()->GetGameInstance());
		Manager->AddDialogueEvent(InstancedObject, Handle);
		InstancedObject->OnCalledEvent(Handle);
		InstancedObject->CalledDialogueID = Handle;
	}
}
