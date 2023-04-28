// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/EdGraphNode_Dialogue_Basic.h"
#include "Nodes/DialogueNode_Basic.h"
#include "DialogueEditor.h"
#include "DialogueAssetEditor/SEdNode_Dialogue_Basic.h"

UDialogueNode_Basic* UEdGraphNode_Dialogue_Basic::GetDialogueBasicNode() const
{
	return Cast<UDialogueNode_Basic>(DialogueNode);
}

FString UEdGraphNode_Dialogue_Basic::GetDialoguerName() const
{
	FString ReturnString;
	UDialogueNode_Basic* Node = GetDialogueBasicNode();
	if (Node) {
		ReturnString = Node->GetDialoguerName_InEditor();
	}
	return ReturnString;
}

FString UEdGraphNode_Dialogue_Basic::GetDialoguerName_Original() const
{
	FString ReturnString;
	UDialogueNode_Basic* Node = GetDialogueBasicNode();
	if (Node) {
		ReturnString = Node->GetDialoguerName_Original_InEditor();
	}
	return ReturnString;
}

FString UEdGraphNode_Dialogue_Basic::GetDialogueString() const
{
	FString ReturnString;
	UDialogueNode_Basic* Node = GetDialogueBasicNode();
	if (Node) {
		ReturnString = Node->GetDialogueString_InEditor();
	}
	return ReturnString;
}

FString UEdGraphNode_Dialogue_Basic::GetDialogueString_Original() const
{
	FString ReturnString;
	UDialogueNode_Basic* Node = GetDialogueBasicNode();
	if (Node) {
		ReturnString = Node->GetDialogueString_Original_InEditor();
	}
	return ReturnString;
}

void UEdGraphNode_Dialogue_Basic::TryImportDialogueString()
{
	UDialogueNode_Basic* Node = GetDialogueBasicNode();
	if (Node)
	{
		Node->ImportDialogue();
	}
	else {
		LOG_ERROR(TEXT("Can't Importy Dialogue String because can't find Node."));
	}
}

void UEdGraphNode_Dialogue_Basic::ChangeDialogueTextStyle(const TSharedPtr<class FSlateStyleSet>& NewStyleSet, TArray<TSharedRef<class ITextDecorator>>& NewDeco, const FTextBlockStyle& DefaultStyle)
{
	SEdNode_Dialogue_Basic* SEdBasic = (SEdNode_Dialogue_Basic*)SEdNode;
	if (SEdBasic == nullptr) {
		LOG_ERROR(TEXT("Can't find SEdNode from UEdGraphNode_Dialogue_Baisic"));
		return;
	}
	
	SEdBasic->ChangeDialogueTextStyle(NewStyleSet, NewDeco, DefaultStyle);
	
}
