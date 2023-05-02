#pragma once

#include "CoreMinimal.h"
#include "AutoLayoutStrategy_DE.h"
#include "TreeLayoutStrategy_DE.generated.h"

UCLASS()
class UTreeLayoutStrategy_DE : public UAutoLayoutStrategy_DE
{
	GENERATED_BODY()
public:
	UTreeLayoutStrategy_DE();
	virtual ~UTreeLayoutStrategy_DE();

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
