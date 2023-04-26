// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "DialogueStructs.h"
#include "EdSchema_Dialogue.generated.h"

/**
 * 
 */
 class UEdGraphNode_Dialogue;
 class UEdGraphNode_DialogueEdge;

 USTRUCT()
 struct FEdSchemaAction_Dialogue_NewNode : public FEdGraphSchemaAction
 {
	 GENERATED_USTRUCT_BODY();
 public:
	 FEdSchemaAction_Dialogue_NewNode() : NodeTemplate(nullptr) {}

	 FEdSchemaAction_Dialogue_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		 : FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr) {}

	 virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	 virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	 UEdGraphNode_Dialogue* NodeTemplate;
 };

 USTRUCT()
struct FEdSchemaAction_Dialogue_NewEdge : public FEdGraphSchemaAction
{
	 GENERATED_USTRUCT_BODY();
 public:
	 FEdSchemaAction_Dialogue_NewEdge() : NodeTemplate(nullptr) {}

	 FEdSchemaAction_Dialogue_NewEdge(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		 : FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr) {}

	 virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	 virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	 UEdGraphNode_DialogueEdge* NodeTemplate;
 };

UCLASS()
class UEdSchema_Dialogue : public UEdGraphSchema
{
	GENERATED_BODY()

private:
	TSubclassOf<UEdGraphNode_Dialogue> GetEdDialogueNodeSubclass(EDialogueNodeType NodeType) const;

protected:
	virtual FText GetNodeCategoryName() const;

public:
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	void CreateStartNode(UEdGraph& Graph) const;

	virtual void GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;


	void GetBreakLinkToSubMenuActions(class UToolMenu* Menu, class UEdGraphPin* InGraphPin);

	virtual EGraphType GetGraphType(const UEdGraph* TestEdGraph) const override;

	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
	virtual bool CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const override;

	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;

	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;

	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;

	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;

	virtual UEdGraphPin* DropPinOnNode(UEdGraphNode* InTargetNode, const FName& InSourcePinName, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection) const override;

	virtual bool SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const override;

	virtual bool IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const override;

	virtual int32 GetCurrentVisualizationCacheID() const override;

	virtual void ForceVisualizationCacheClear() const override;


private:
	static int32 CurrentCacheRefreshID;
};
