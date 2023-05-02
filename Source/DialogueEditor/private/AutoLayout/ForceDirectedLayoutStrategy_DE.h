#pragma once

#include "CoreMinimal.h"
#include "AutoLayoutStrategy_DE.h"
#include "ForceDirectedLayoutStrategy_DE.generated.h"

UCLASS()
class UForceDirectedLayoutStrategy_DE : public UAutoLayoutStrategy_DE
{
	GENERATED_BODY()
public:
	UForceDirectedLayoutStrategy_DE();
	virtual ~UForceDirectedLayoutStrategy_DE();

	virtual void Layout(UEdGraph* EdGraph) override;

protected:
	virtual FBox2D LayoutOneTree(UDialogueNode* RootNode, const FBox2D& PreTreeBound);

protected:
	bool bRandomInit;
	float InitTemperature;
	float CoolDownRate;
};
