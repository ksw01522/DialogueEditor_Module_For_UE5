// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/DialogueNode_Start.h"

#define LOCTEXT_NAMESPACE "DialogueNode"

UDialogueNode_Start::UDialogueNode_Start()
{
#if WITH_EDITORONLY_DATA
	BackgroundColor = FLinearColor(0,0.25,1);
	ContextMenuName = LOCTEXT("Dialogue Node_Start Title", "Dialogue Start");
#endif
	DialogueNodeType = EDialogueNodeType::Start;
}

#if WITH_EDITOR
FText UDialogueNode_Start::GetNodeTitle()
{
	return LOCTEXT("Dialouge Start Node Name", "Start");
}
#endif
#undef LOCTEXT_NAMESPACE