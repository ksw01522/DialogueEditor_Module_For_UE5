// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/EdGraphNode_Dialogue_Basic.h"
#include "DialogueEditor.h"
#include "DialogueAssetEditor/SEdNode_Dialogue_Basic.h"
#include "Components/RichTextBlockDecorator.h"
#include "SRichTextBlockDecorator.h"

UDialogueNode_Basic* UEdGraphNode_Dialogue_Basic::GetDialogueBasicNode() const
{
	UDialogueNode_Basic* BasicNode = Cast<UDialogueNode_Basic>(DialogueNode);
	ensure(BasicNode != nullptr);
	return BasicNode;
}

UDataTable* UEdGraphNode_Dialogue_Basic::GetDialogueTextStyleSet() const
{
	UDialogueNode_Basic* BasicNode = GetDialogueBasicNode();

	return BasicNode->GetDialogueTextStyleSet();
}

ERichTextBlockType UEdGraphNode_Dialogue_Basic::GetTextBlockType() const
{
	return GetDialogueBasicNode()->GetRichTextBlockType();
}

TArray<TSubclassOf<USRichTextBlockDecorator>> UEdGraphNode_Dialogue_Basic::GetSlateDecoClasses() const
{
	return GetDialogueBasicNode()->GetSlateDecoClasses();
}

TArray<TSubclassOf<URichTextBlockDecorator>> UEdGraphNode_Dialogue_Basic::GetUMGDecoClasses() const
{
	return GetDialogueBasicNode()->GetUMGDecoClasses();
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

void UEdGraphNode_Dialogue_Basic::TryImportDialogueString() const
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

void UEdGraphNode_Dialogue_Basic::OnChangedDialogueStyleFunction()
{
	ensure(SEdNode_Basic != nullptr);

	SEdNode_Basic->UpdateRichTextStyle();
}

void UEdGraphNode_Dialogue_Basic::SetDialogueNode(UDialogueNode* NewNode)
{
	DialogueNode = NewNode;
	Cast<UDialogueNode_Basic>(DialogueNode)->OnChangedDialogueStyle.BindDynamic(this, &UEdGraphNode_Dialogue_Basic::OnChangedDialogueStyleFunction);
}

