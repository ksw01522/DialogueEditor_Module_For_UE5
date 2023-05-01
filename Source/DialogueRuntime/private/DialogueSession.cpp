// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSession.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "DialogueHeaders.h"

#define LOCTEXT_NAMESPACE "DialogueSession"

UDialogueSession::UDialogueSession()
{
	bEdgeEnabled = true;
	bHideNodeType = ECustomNodeTypeHidden::Hidden;

#if WITH_EDITORONLY_DATA
	PreviewLanguage = EDialogueLanguage::Korean;
	EdGraph = nullptr;
	bCanRenameNode = true;

#endif
}


void UDialogueSession::ClearGraph()
{
	for (int i = 0; i < AllNodes.Num(); ++i)
	{
		UDialogueNode* Node = AllNodes[i];
		if (Node)
		{
			Node->ParentNodes.Empty();
			Node->ChildrenNodes.Empty();
			Node->Edges.Empty();
		}
	}

	AllNodes.Empty();
}



UDialogueNode_Start* UDialogueSession::GetStartNode() const
{
	return StartNode;
}





#if WITH_EDITOR
void UDialogueSession::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == "PreviewLanguage") { OnChaged_PreviewLanguage(); }
	else if (PropertyChangedEvent.GetPropertyName() == "DialogueTextStyleSet" || PropertyChangedEvent.GetPropertyName() == "DecoratorClasses") {
		for (auto Node : AllNodes)
		{
			Cast<UDialogueNode>(Node)->OnChangedDialogueTextStyle();
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}




void UDialogueSession::OnChaged_PreviewLanguage()
{
	for (auto Node : AllNodes)
	{
		Cast<UDialogueNode>(Node)->OnChanged_PreviewLanguage();
	}
}
#endif

#undef LOCTEXT_NAMESPACE


