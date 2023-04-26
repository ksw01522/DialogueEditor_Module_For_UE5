// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueBFL.h"
#include "DialogueNode.h"
#include "Nodes/DialogueNode_Basic.h"
#include "Nodes/DialogueNode_Question.h"
#include "DialogueRuntime.h"
#include "Kismet/GameplayStatics.h"


UDialogueManager* UDialogueBFL::GetDialogueManager()
{
	UWorld* World = GEngine->GameViewport->GetWorld();
	if (World == nullptr) return nullptr;
	UDialogueManager* DialogueManager = UGameInstance::GetSubsystem<UDialogueManager>(World->GetGameInstance());
	if (!IsValid(DialogueManager)) {
		LOG_ERROR(TEXT("Can't find Dialogue Manager."));
		return nullptr;
	}
	return DialogueManager;
}
