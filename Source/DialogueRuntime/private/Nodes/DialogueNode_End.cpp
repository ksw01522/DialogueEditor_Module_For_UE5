// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/DialogueNode_End.h"

#define LOCTEXT_NAMESPACE "DialogueNode"


UDialogueNode_End::UDialogueNode_End()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("Dialogue Node End Title", "End");
#endif
	DialogueNodeType = EDialogueNodeType::End;
}
#if WITH_EDITOR
FText UDialogueNode_End::GetNodeTitle()
{
	return LOCTEXT("Dialouge End Node Name", "End");
}
#endif
#undef LOCTEXT_NAMESPACE
