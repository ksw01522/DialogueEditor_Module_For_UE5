#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "DialogueSession.h"
#include "DialogueAssetEditor/EdGraph_DialogueSession.h"
#include "DialogueAssetEditor/EdGraphNode_Dialogue.h"
#include "DialogueAssetEditor/EdGraphNode_DialogueEdge.h"
#include "DialogueAssetEditor/Settings_DialogueEditor.h"
#include "AutoLayoutStrategy.generated.h"

UCLASS(abstract)
class UAutoLayoutStrategy : public UObject
{
	GENERATED_BODY()
public:
	UAutoLayoutStrategy();
	virtual ~UAutoLayoutStrategy();

	virtual void Layout(UEdGraph* G) {};

	class UDialogueEditorSettings* Settings;

protected:
	int32 GetNodeWidth(UEdGraphNode_Dialogue* EdNode);

	int32 GetNodeHeight(UEdGraphNode_Dialogue* EdNode);

	FBox2D GetNodeBound(UEdGraphNode* EdNode);

	FBox2D GetActualBounds(UDialogueNode* RootNode);

	virtual void RandomLayoutOneTree(UDialogueNode* RootNode, const FBox2D& Bound);

protected:
	UDialogueSession* Graph;
	UEdGraph_DialogueSession* EdGraph;
	int32 MaxIteration;
	int32 OptimalDistance;
};
