#pragma once

#include "CoreMinimal.h"
#include "AutoLayoutStrategy.h"
#include "TreeLayoutStrategy.generated.h"

UCLASS()
class UTreeLayoutStrategy : public UAutoLayoutStrategy
{
	GENERATED_BODY()
public:
	UTreeLayoutStrategy();
	virtual ~UTreeLayoutStrategy();

	virtual void Layout(UEdGraph* EdGraph) override;

protected:
	void InitPass(UDialogueNode* RootNode, const FVector2D& Anchor);
	bool ResolveConflictPass(UDialogueNode* Node);

	bool ResolveConflict(UDialogueNode* LRoot, UDialogueNode* RRoot);

	void GetLeftContour(UDialogueNode* RootNode, int32 Level, TArray<UEdGraphNode_Dialogue*>& Contour);
	void GetRightContour(UDialogueNode* RootNode, int32 Level, TArray<UEdGraphNode_Dialogue*>& Contour);
	
	void ShiftSubTree(UDialogueNode* RootNode, const FVector2D& Offset);

	void UpdateParentNodePosition(UDialogueNode* RootNode);
};
