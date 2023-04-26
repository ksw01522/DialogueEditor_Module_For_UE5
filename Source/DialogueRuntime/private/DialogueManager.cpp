// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueManager.h"
#include "Templates/SharedPointer.h"
#include "DialogueRuntime.h"
#include "DialogueHeaders.h"
#include "Components/AudioComponent.h"

////////////////////////////////////////// FActingDialogueData /////////////////////////////////////////////
FActingDialogueData::FActingDialogueData(TArray<UDialoguerComponent*> NewDialoguers, UDialogueSession* Session)
{
	Dialoguers.Empty();
	DialogueEvents.Empty();
	DialogueSession = Session;
	if (DialogueSession.IsValid()) {
		CurrentNode = Session->GetStartNode();
	} else {CurrentNode.Reset(); }
}

bool FActingDialogueData::IsValidDialogue() const
{
	return (DialogueSession.IsValid() && CurrentNode.IsValid());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////// Add And Remove Dialogue ///////////////////////////////////////////
FActingDialogueHandle UDialogueManager::EnterDialogue(TArray<UDialoguerComponent*> Dialoguers, UDialogueSession* Session)
{
	if(Session == nullptr){ return FActingDialogueHandle(); }

	FActingDialogueHandle Handle(GDialogueHandleID++);
	FActingDialogueData Data(Dialoguers, Session);

	ActingDialogueMap.Add(Handle, Data);

	for (auto Dialoguer : Dialoguers) {
		Dialoguer->OnEnteredDialogue(Handle);
	}

	return Handle;
}



void UDialogueManager::RemoveDialogue(FActingDialogueHandle Target)
{
	if (!Target.IsValid()) return;

	FActingDialogueData* Data = ActingDialogueMap.Find(Target);
	if(Data == nullptr) return;

	CallEndEvents(Data, true);

	for (auto Dialoguer : Data->Dialoguers) {
		Dialoguer->DialogueHandle.Invalidate();
	}

	ActingDialogueMap.Remove(Target);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////// Get ////////////////////////////////////////////////////////

// 등록된 대화 데이터 가져오기
FActingDialogueData* UDialogueManager::GetActingDialogueData(FActingDialogueHandle& Handle)
{
	if(CheckDialogueFromHandle(Handle)) {return nullptr;}
	
	FActingDialogueData* ReturnData = ActingDialogueMap.Find(Handle);

	return ReturnData;
}

bool UDialogueManager::IsCanEnterNextNode(FActingDialogueHandle& Handle)
{
	FActingDialogueData* Data = GetActingDialogueData(Handle);
	if(Data == nullptr) return false;

	for (auto Event : Data->DialogueEvents) {
		if(!IsValid(Event)) continue;

		if(!Event->CanEnterNextNode()) return false;
	}

	return true;
}

// 대화사람들 가져오기
bool UDialogueManager::GetDialoguersInDialog(TArray<UDialoguerComponent*>& OutDialoguers, FActingDialogueHandle& Handle)
{
	FActingDialogueData* Data = GetActingDialogueData(Handle);
	if (Data == nullptr) { return false; }

	for (auto Dialoguer : Data->Dialoguers) {
		if(Dialoguer.IsValid()) OutDialoguers.Add(Dialoguer.Get());
	}

	return true;
}



// 대화 요소 : 말하는 사람 이름과 대화 내용 가져오기
bool UDialogueManager::GetElementFromNode(TArray<FDialogueElement>& OutElements, UDialogueNode_Basic* BasicNode) const
{
	if(!IsValid(BasicNode)) return false;

	FDialogueElement TempElement;
	TempElement.Name = BasicNode->GetDialoguerName(CurrentLanguage);
	TempElement.DialogString = BasicNode->GetDialogueString(CurrentLanguage);

	OutElements.Add(TempElement);

	return true;
}

void UDialogueManager::GetElementsFromData(TArray<FDialogueElement>& OutElements, FActingDialogueData* Data) const
{
	if(Data == nullptr || !Data->IsValidDialogue()) return;

	EDialogueNodeType NodeType = Data->CurrentNode->GetDialogueNodeType();

	UDialogueNode_Basic* BasicNode = Cast<UDialogueNode_Basic>(Data->CurrentNode);
	if (BasicNode != nullptr) {
		
		GetElementFromNode(OutElements, BasicNode);
	}

	if (NodeType == EDialogueNodeType::Basic) {
		if(BasicNode == nullptr) return;

	}
	else if (NodeType == EDialogueNodeType::Question) {
		UDialogueNode_Question* QuestionNode = Cast<UDialogueNode_Question>(Data->CurrentNode);
		if(QuestionNode == nullptr) return;

		FDialogueElement TempElement;
		TempElement.Name = QuestionNode->GetDialoguerName(CurrentLanguage);
		TempElement.DialogString = QuestionNode->GetDialogueString(CurrentLanguage);
		OutElements.Add(TempElement);

		for (UDialogueNode* ChildNode : QuestionNode->ChildrenNodes) {
			UDialogueNode_Answer* AnswerNode = Cast<UDialogueNode_Answer>(ChildNode);
			if (AnswerNode == nullptr) {
				LOG_ERROR(TEXT("Question Node's Child Node Is Not AnswerNode from %s"), *Data->DialogueSession->GetName());
				continue;
			}

			FDialogueElement TempAnswerElement;
			TempAnswerElement.Name = AnswerNode->GetDialoguerName(CurrentLanguage);
			TempAnswerElement.DialogString = AnswerNode->GetDialogueString(CurrentLanguage);
			OutElements.Add(TempAnswerElement);
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool UDialogueManager::CheckDialogueFromHandle(FActingDialogueHandle& Handle)
{
	if(!Handle.IsValid()) return false;

	FActingDialogueData* Data = ActingDialogueMap.Find(Handle);

	if (Data == nullptr) {
		LOG_ERROR(TEXT("Acting Dialogue Data Is nullptr."));
		Handle.Invalidate();
		return false;
	}

	if (!Data->IsValidDialogue()) {
		LOG_ERROR(TEXT("Dialogue Data Is Not Valid."));
		RemoveDialogue(Handle);
		return false;
	}

	return true;
}




EDialogueNodeType UDialogueManager::EnterNextNode(TArray<FDialogueElement>& OutElements, FActingDialogueHandle& Handle)
{
	FActingDialogueData* Data = GetActingDialogueData(Handle);
	if(Data == nullptr) { return EDialogueNodeType::None; }
	if(!IsCanEnterNextNode(Handle)) return EDialogueNodeType::None;

	if (Data->CurrentNode->GetDialogueNodeType() == EDialogueNodeType::Question) {
		LOG_ERROR(TEXT("Do Not Call this Function From Question Node."));
		return EDialogueNodeType::None;
	}

	CallEndEvents(Data, true);

	if (Data->CurrentNode->ChildrenNodes.IsValidIndex(0)) { Data->CurrentNode = Cast<UDialogueNode>(Data->CurrentNode->ChildrenNodes[0]); }
	else { Data->CurrentNode.Reset(); }

	if (!Data->CurrentNode.IsValid()) {
		RemoveDialogue(Handle);
		return EDialogueNodeType::End;
	}

	switch (Data->CurrentNode->GetDialogueNodeType()) {
		case EDialogueNodeType::None:
		case EDialogueNodeType::Start:
		case EDialogueNodeType::Answer:
			LOG_ERROR(TEXT("%s Session is abnormal. take a look Session"), *Data->DialogueSession->GetName());
			RemoveDialogue(Handle);
			return EDialogueNodeType::None;
		break;

		case EDialogueNodeType::Basic:
		case EDialogueNodeType::Question:
			GetElementsFromData(OutElements, Data);
			Data->CurrentNode->CallEvents(Handle);
			return Data->CurrentNode->GetDialogueNodeType();
		break;

		case EDialogueNodeType::End:
			Data->CurrentNode->CallEvents(Handle);
			RemoveDialogue(Handle);
			return EDialogueNodeType::End;
		break;
	}


	return EDialogueNodeType::None;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////// Events ///////////////////////////////////////////////////

bool UDialogueManager::AddDialogueEvent(UDialogueEvent* NewEvent, FActingDialogueHandle& Handle)
{
	if(!IsValid(NewEvent)) return false;

	FActingDialogueData* Data = GetActingDialogueData(Handle);
	if(Data == nullptr) return false;

	Data->DialogueEvents.Add(NewEvent);

	return true;
}

bool UDialogueManager::RemoveDialogueEvent(UDialogueEvent* RemoveEvent, FActingDialogueHandle& Handle)
{
	FActingDialogueData* Data = GetActingDialogueData(Handle);
	if(Data == nullptr) return false;

	if(Data->DialogueEvents.Remove(RemoveEvent) <= 0) return false;

	return true;
}


void UDialogueManager::RemoveAllEvents(FActingDialogueHandle& Handle)
{
	FActingDialogueData* Data = GetActingDialogueData(Handle);
	RemoveAllEvents(Data);
}

void UDialogueManager::RemoveAllEvents(FActingDialogueData* Data)
{
	if(Data == nullptr) return;
	Data->DialogueEvents.Empty();
}

void UDialogueManager::CallEndEvents(FActingDialogueData* Data, bool bIsCancelled)
{
	if(Data == nullptr) return;

	for (auto Event : Data->DialogueEvents) {
		Event->CallEndEvent(bIsCancelled);
	}

	RemoveAllEvents(Data);
}
