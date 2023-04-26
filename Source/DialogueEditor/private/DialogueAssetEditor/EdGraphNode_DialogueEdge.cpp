// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/EdGraphNode_DialogueEdge.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
#include "DialogueNode.h"

#define LOCTEXT_NAMESPACE "EdGraphNode_DialogueEdge"

UEdGraphNode_DialogueEdge::UEdGraphNode_DialogueEdge()
{
	bCanRenameNode = true;
}

void UEdGraphNode_DialogueEdge::SetEdge(UDialogueEdge* Edge)
{
	DialogueEdge = Edge;
}

void UEdGraphNode_DialogueEdge::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Edge"), FName(), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Edge"), FName(), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UEdGraphNode_DialogueEdge::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (DialogueEdge)
	{
		return DialogueEdge->GetNodeTitle();
	}

	return FText();
}

void UEdGraphNode_DialogueEdge::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		Modify();

		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UEdGraphNode_DialogueEdge::PrepareForCopying()
{
	DialogueEdge->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdGraphNode_DialogueEdge::CreateConnections(UEdGraphNode_Dialogue* Start, UEdGraphNode_Dialogue* End)
{
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	Start->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(Start->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	End->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(End->GetInputPin());
}

UEdGraphNode_Dialogue* UEdGraphNode_DialogueEdge::GetStartNode()
{
	if (0 < Pins[0]->LinkedTo.Num())
	{
		return Cast<UEdGraphNode_Dialogue>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

UEdGraphNode_Dialogue* UEdGraphNode_DialogueEdge::GetEndNode()
{
	if (0 < Pins[1]->LinkedTo.Num())
	{
		return Cast<UEdGraphNode_Dialogue>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

#undef LOCTEXT_NAMESPACE