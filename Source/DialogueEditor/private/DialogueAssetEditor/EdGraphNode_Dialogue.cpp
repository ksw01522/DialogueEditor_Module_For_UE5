// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
#include "DialogueAssetEditor/EdGraph_DialogueSession.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphNode_Dialogue.h"
#include "DialogueNode.h"
#include "Settings_DialogueEditor.h"
#include "SEdNode_DialogueNode.h"

UEdGraphNode_Dialogue::UEdGraphNode_Dialogue()
{
	bCanRenameNode = true;
}

UEdGraph_DialogueSession* UEdGraphNode_Dialogue::GetDialogueEdGraph()
{
	return Cast<UEdGraph_DialogueSession>(GetGraph());
}

void UEdGraphNode_Dialogue::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

FText UEdGraphNode_Dialogue::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (DialogueNode == nullptr)
	{
		return Super::GetNodeTitle(TitleType);
	}
	else
	{
		return DialogueNode->GetNodeTitle();
	}
}



void UEdGraphNode_Dialogue::PrepareForCopying()
{
	DialogueNode->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdGraphNode_Dialogue::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

FLinearColor UEdGraphNode_Dialogue::GetBackgroundColor() const
{
	return DialogueNode == nullptr ? FLinearColor::Black : DialogueNode->GetBackgroundColor();
}

UEdGraphPin* UEdGraphNode_Dialogue::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UEdGraphNode_Dialogue::GetOutputPin() const
{
	return Pins[1];
}

UDialogueNode* UEdGraphNode_Dialogue::GetDialogueNode() const
{
	return DialogueNode;
}

void UEdGraphNode_Dialogue::SetDialogueNode(UDialogueNode* InNode)
{
	DialogueNode = InNode;
}


void UEdGraphNode_Dialogue::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
}

FText UEdGraphNode_Dialogue::GetNodeDescription()
{
	return IsValid(DialogueNode) ? DialogueNode->GetDescription() : FText();
}



