// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueNode.h"
#include "Events/DialogueEvent.h"
#include "DialogueSession.h"
#include "DialogueRuntime.h"

#define LOCTEXT_NAMESPACE "DialogueNode"

UDialogueNode::UDialogueNode()
{
#if WITH_EDITORONLY_DATA
	ContextMenuName = LOCTEXT("Dialogue Node Title", "Dialogue Node");
#endif
	DialogueNodeType = EDialogueNodeType::None;
}



UDialogueSession* UDialogueNode::GetDialogueSession() const
{
	return DialogueSession;
}

void UDialogueNode::CallEvents(FActingDialogueHandle& Handle)
{
	for (auto Event : EnterEvents) {
		Event->CallEvent(Handle);
	}
}

UDialogueEdge* UDialogueNode::GetEdge(UDialogueNode* ChildNode)
{
	return nullptr;
}

bool UDialogueNode::IsLeafNode() const
{
	return ChildrenNodes.Num() == 0;
}
 

#if WITH_EDITOR

FText UDialogueNode::GetDescription_Implementation() const
{
	return FText::GetEmpty();
}

bool UDialogueNode::IsNameEditable() const
{
	return false;
}

FLinearColor UDialogueNode::GetBackgroundColor() const
{
	return BackgroundColor;
}

FText UDialogueNode::GetNodeTitle()
{
	return FText::GetEmpty();
}


bool UDialogueNode::CanCreateConnection(UDialogueNode* Other, FText& ErrorMessage)
{
	return true;
}

bool UDialogueNode::CanCreateConnectionTo(UDialogueNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage)
{
	if(!CanCreateConnection(Other, ErrorMessage)) return false;
	if (Other->GetDialogueNodeType() == EDialogueNodeType::Start) return false;

	if (ChildrenLimitType == ECustomNodeLimit::Limited && NumberOfChildrenNodes >= ChildrenLimit)
	{
		ErrorMessage = FText::FromString("Children limit exceeded");
		return false;
	}

	return true;
}

bool UDialogueNode::CanCreateConnectionFrom(UDialogueNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage)
{
	if (!CanCreateConnection(Other, ErrorMessage)) return false;
	if (Other->GetDialogueNodeType() == EDialogueNodeType::End) return false;

	if (ParentLimitType == ECustomNodeLimit::Limited && NumberOfParentNodes >= ParentLimit)
	{
		ErrorMessage = FText::FromString("Parent limit exceeded");
		return false;
	}

	return true;
}



#endif

#undef LOCTEXT_NAMESPACE


