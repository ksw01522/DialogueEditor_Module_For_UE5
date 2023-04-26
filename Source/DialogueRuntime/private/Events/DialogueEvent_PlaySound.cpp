// Fill out your copyright notice in the Description page of Project Settings.


#include "Events/DialogueEvent_PlaySound.h"
#include "Kismet/GameplayStatics.h"
#include "DialogueRuntime.h"
#include "DialoguerComponent.h"
#include "Components/AudioComponent.h"
#include "DialogueBFL.h"


void UDialogueEvent_PlaySound::Client_PlaySound2D_Implementation(const TArray<UDialoguerComponent*>& Dialoguers, FActingDialogueHandle Handle)
{
	UAudioComponent* NewAudio = UGameplayStatics::SpawnSound2D(GetWorld(), SoundSource);
	InstanceAudio = NewAudio;
}

void UDialogueEvent_PlaySound::Client_PlaySoundFromActor_Implementation(const TArray<UDialoguerComponent*>& Dialoguers, FActingDialogueHandle Handle)
{
	for (auto Dialoguer : Dialoguers) {
		if(!IsValid(Dialoguer) || Dialoguer->GetDialoguerID() != LocationDialoguerID) continue;
		UAudioComponent* NewAudio =  UGameplayStatics::SpawnSoundAttached(SoundSource, Dialoguer->GetOwner()->GetRootComponent(), NAME_None, FVector(0,0,0), EAttachLocation::KeepRelativeOffset, true);
		InstanceAudio = NewAudio;
	}
}

void UDialogueEvent_PlaySound::OnEndEvent_Implementation(bool bIsCancelled)
{
	if(!bWantStopWhenEventEnd || !InstanceAudio.IsValid()) return;

	InstanceAudio->Stop();
	InstanceAudio->DestroyComponent();
	MarkAsGarbage();
}

void UDialogueEvent_PlaySound::OnCalledEvent_Implementation(FActingDialogueHandle& Handle)
{
	if (SoundSource == nullptr) {
		LOG_WARNING(TEXT("not valid SoundSource."));
		return;
	}
	
	TArray<UDialoguerComponent*> Dialoguers;
	bool bCanGetDialoguers = UDialogueBFL::GetDialogueManager()->GetDialoguersInDialog(Dialoguers, Handle);
	if (!bCanGetDialoguers) {
		LOG_ERROR(TEXT("Can't Get Dialoguers."));
		return;
	}


	if (PlaySoundType == EPlaySoundType::FromActor) { Client_PlaySoundFromActor(Dialoguers, Handle); }
	else if (PlaySoundType == EPlaySoundType::Anywhere) { Client_PlaySound2D(Dialoguers, Handle); }
	else {
		LOG_ERROR(TEXT("Can't Find PlaySoundType."));
	}
	
}
