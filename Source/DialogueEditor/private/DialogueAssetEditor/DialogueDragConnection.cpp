// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueAssetEditor/DialogueDragConnection.h"
#include "Widgets/SBoxPanel.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Images/SImage.h"
#include "EdGraph/Edgraph.h"
#include "SGraphPanel.h"
#include "ScopedTransaction.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
#include "DialogueAssetEditor/DialogueEditorStyle.h"


TSharedRef<FDialogueDragConnection> FDialogueDragConnection::New(const TSharedRef<SGraphPanel>& GraphPanel, const FDraggedPinTable& DraggedPins)
{
	TSharedRef<FDialogueDragConnection> Operation = MakeShareable(new FDialogueDragConnection(GraphPanel, DraggedPins));
	Operation->Construct();

	return Operation;
}

void FDialogueDragConnection::OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
	GraphPanel->OnStopMakingConnection();

	Super::OnDrop(bDropWasHandled, MouseEvent);
}

void FDialogueDragConnection::HoverTargetChanged()
{
	TArray<FPinConnectionResponse> UniqueMessages;

	if (UEdGraphPin* TargetPinObj = GetHoveredPin())
	{
		TArray<UEdGraphPin*> ValideSourcePins;
		ValidateGraphPinList(/*out*/ ValideSourcePins);

		for (UEdGraphPin* StartingPinObj : ValideSourcePins)
		{
			UEdGraph* GraphObj = StartingPinObj->GetOwningNode()->GetGraph();

			const FPinConnectionResponse Response = GraphObj->GetSchema()->CanCreateConnection(StartingPinObj, TargetPinObj);

			if (Response.Response == ECanCreateConnectionResponse::CONNECT_RESPONSE_DISALLOW)
			{
				TSharedPtr<SGraphNode> NodeWidget = TargetPinObj->GetOwningNode()->DEPRECATED_NodeWidget.Pin();
				if (NodeWidget.IsValid())
				{
					NodeWidget->NotifyDisallowedPinConnection(StartingPinObj, TargetPinObj);
				}

				UniqueMessages.AddUnique(Response);
			}

		}
	}
	else if (UEdGraphNode_Dialogue* TargetNodeObj = Cast <UEdGraphNode_Dialogue>(GetHoveredNode()))
	{
		TArray<UEdGraphPin*> ValideSourcePins;
		ValidateGraphPinList(/*out*/ ValideSourcePins);

		for (UEdGraphPin* StartingPinObj : ValideSourcePins)
		{
			FPinConnectionResponse Response;
			FText ResponseText;

			const UEdGraphSchema* Schema = StartingPinObj->GetSchema();
			UEdGraphPin* TargetPin = TargetNodeObj->GetInputPin();

			if (Schema && TargetPin)
			{
				Response = Schema->CanCreateConnection(StartingPinObj, TargetPin);
				if (Response.Response == ECanCreateConnectionResponse::CONNECT_RESPONSE_DISALLOW)
				{
					TSharedPtr<SGraphNode> NodeWidget = TargetPin->GetOwningNode()->DEPRECATED_NodeWidget.Pin();
					if (NodeWidget.IsValid())
					{
						NodeWidget->NotifyDisallowedPinConnection(StartingPinObj, TargetPinObj);
					}
				}
			}
			else
			{
#define LOCTEXT_NAMESPACE "AssetSchema_GenericGraph"
				Response = FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinError", "Not a valid UGenericGraphEdNode"));
#undef LOCTEXT_NAMESPACE
			}

			UniqueMessages.AddUnique(Response);
		}
	}
	//On Horvered Graph
	else if (UEdGraph* CurrentHoveredGraph = GetHoveredGraph())
	{
		TArray<UEdGraphPin*> ValideSourcePins;
		ValidateGraphPinList(/*out*/ ValideSourcePins);

		for (UEdGraphPin* StartingPinObj : ValideSourcePins)
		{
			FPinConnectionResponse Response = CurrentHoveredGraph->GetSchema()->CanCreateNewNodes(StartingPinObj);
			if (!Response.Message.IsEmpty())
			{
				UniqueMessages.AddUnique(Response);
			}
		}
	}

	if (UniqueMessages.Num() == 0)
	{
		SetSimpleFeedbackMessage(
			FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.NewNode")),
			FLinearColor::White,
			NSLOCTEXT("GraphEditor.Feedback", "PlaceNewNode", "Place a new node.")
			);
	}
	else
	{
		TSharedRef<SVerticalBox> FeedbackBox = SNew(SVerticalBox);
		for (auto ResponseIt = UniqueMessages.CreateConstIterator(); ResponseIt; ++ResponseIt)
		{
			// Determine the icon
			const FSlateBrush* StatusSymbol = NULL;

			switch (ResponseIt->Response)
			{
			case CONNECT_RESPONSE_MAKE:
			case CONNECT_RESPONSE_BREAK_OTHERS_A:
			case CONNECT_RESPONSE_BREAK_OTHERS_B:
			case CONNECT_RESPONSE_BREAK_OTHERS_AB:
				StatusSymbol = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK"));
				break;

			case CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE:
				StatusSymbol = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.ViaCast"));
				break;

			case CONNECT_RESPONSE_DISALLOW:
			default:
				StatusSymbol = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));
				break;
			}

			// Add a new message row
			FeedbackBox->AddSlot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(3.0f)
				.VAlign(VAlign_Center)
				[
					SNew(SImage).Image(StatusSymbol)
				]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock).Text(ResponseIt->Message)
				]
				];
		}

		SetFeedbackMessage(FeedbackBox);
	}
}



void FDialogueDragConnection::OnDragged(const FDragDropEvent& DragDropEvent)
{
	FVector2D TargetPosition = DragDropEvent.GetScreenSpacePosition();

	// Reposition the info window wrt to the drag
	CursorDecoratorWindow->MoveWindowTo(TargetPosition + DecoratorAdjust);
	// Request the active panel to scroll if required
	GraphPanel->RequestDeferredPan(TargetPosition);

}

void FDialogueDragConnection::ValidateGraphPinList(TArray<UEdGraphPin*>& OutValidPins)
{
	OutValidPins.Empty(DraggingPins.Num());
	for (const FGraphPinHandle& PinHandle : DraggingPins)
	{
		if (UEdGraphPin* GraphPin = PinHandle.GetPinObj(*GraphPanel))
		{
			OutValidPins.Add(GraphPin);
		}
	}
}

FDialogueDragConnection::FDialogueDragConnection(const TSharedRef<SGraphPanel>& GraphPanelIn, const FDraggedPinTable& DraggedPinsIn)
	:	GraphPanel(GraphPanelIn),
		DraggingPins(DraggedPinsIn),
		DecoratorAdjust(FSlateApplication::Get().GetCursorSize())
{
	if (DraggingPins.Num() > 0)
	{
		const UEdGraphPin* PinObj = FDraggedPinTable::TConstIterator(DraggedPinsIn)->GetPinObj(*GraphPanelIn);
		if (PinObj && PinObj->Direction == EGPD_Input)
		{
			DecoratorAdjust *= FVector2D(-1.0f, 1.0f);
		}

		for (const FGraphPinHandle& DraggedPin : DraggedPinsIn)
		{
			GraphPanelIn->OnBeginMakingConnection(DraggedPin);
		}
	}
}

FReply FDialogueDragConnection::DroppedOnPin(FVector2D ScreenPosition, FVector2D GraphPosition)
{
	TArray<UEdGraphPin*> ValidSourcePins;
	ValidateGraphPinList(/*out*/ ValidSourcePins);

	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_CreateConnection", "Create Pin Link"));

	UEdGraphPin* PinB = GetHoveredPin();
	bool bError = false;
	TSet<UEdGraphNode*> NodeList;

	for (UEdGraphPin* PinA : ValidSourcePins)
	{
		if ((PinA != NULL) && (PinB != NULL))
		{
			UEdGraph* MyGraphObj = PinA->GetOwningNode()->GetGraph();

			if (MyGraphObj->GetSchema()->TryCreateConnection(PinA, PinB))
			{
				if (!PinA->IsPendingKill())
				{
					NodeList.Add(PinA->GetOwningNode());
				}
				if (!PinB->IsPendingKill())
				{
					NodeList.Add(PinB->GetOwningNode());
				}
			}
		}
		else
		{
			bError = true;
		}
	}

	// Send all nodes that received a new pin connection a notification
	for (auto It = NodeList.CreateConstIterator(); It; ++It)
	{
		UEdGraphNode* Node = (*It);
		Node->NodeConnectionListChanged();
	}

	if (bError)
	{
		return FReply::Unhandled();
	}

	return FReply::Handled();
}

FReply FDialogueDragConnection::DroppedOnNode(FVector2D ScreenPosition, FVector2D GraphPosition)
{
	bool bHandledPinDropOnNode = false;
	UEdGraphNode* NodeOver = GetHoveredNode();

	if (NodeOver)
	{
		// Gather any source drag pins
		TArray<UEdGraphPin*> ValidSourcePins;
		ValidateGraphPinList(/*out*/ ValidSourcePins);

		if (ValidSourcePins.Num())
		{
			for (UEdGraphPin* SourcePin : ValidSourcePins)
			{
				// Check for pin drop support
				FText ResponseText;
				if (SourcePin->GetOwningNode() != NodeOver && SourcePin->GetSchema()->SupportsDropPinOnNode(NodeOver, SourcePin->PinType, SourcePin->Direction, ResponseText))
				{
					bHandledPinDropOnNode = true;

					// Find which pin name to use and drop the pin on the node
					const FName PinName = SourcePin->PinFriendlyName.IsEmpty() ? SourcePin->PinName : *SourcePin->PinFriendlyName.ToString();

					const FScopedTransaction Transaction((SourcePin->Direction == EGPD_Output) ? NSLOCTEXT("UnrealEd", "AddInParam", "Add In Parameter") : NSLOCTEXT("UnrealEd", "AddOutParam", "Add Out Parameter"));

					UEdGraphPin* EdGraphPin = NodeOver->GetSchema()->DropPinOnNode(GetHoveredNode(), PinName, SourcePin->PinType, SourcePin->Direction);

					// This can invalidate the source pin due to node reconstruction, abort in that case
					if (SourcePin->GetOwningNodeUnchecked() && EdGraphPin)
					{
						SourcePin->Modify();
						EdGraphPin->Modify();
						SourcePin->GetSchema()->TryCreateConnection(SourcePin, EdGraphPin);
					}
				}

				// If we have not handled the pin drop on node and there is an error message, do not let other actions occur.
				if (!bHandledPinDropOnNode && !ResponseText.IsEmpty())
				{
					bHandledPinDropOnNode = true;
				}
			}
		}
	}
	return bHandledPinDropOnNode ? FReply::Handled() : FReply::Unhandled();
}

FReply FDialogueDragConnection::DroppedOnPanel(const TSharedRef<SWidget>& Panel, FVector2D ScreenPosition, FVector2D GraphPosition, UEdGraph& Graph)
{
	// Gather any source drag pins
	TArray<UEdGraphPin*> PinObjects;
	ValidateGraphPinList(/*out*/ PinObjects);

	// Create a context menu
	TSharedPtr<SWidget> WidgetToFocus = GraphPanel->SummonContextMenu(ScreenPosition, GraphPosition, NULL, NULL, PinObjects);

	// Give the context menu focus
	return (WidgetToFocus.IsValid())
		? FReply::Handled().SetUserFocus(WidgetToFocus.ToSharedRef(), EFocusCause::SetDirectly)
		: FReply::Handled();
}
