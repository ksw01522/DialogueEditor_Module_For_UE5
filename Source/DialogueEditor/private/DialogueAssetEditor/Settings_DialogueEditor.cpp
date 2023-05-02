#include "DialogueAssetEditor/Settings_DialogueEditor.h"

#include "Styling/SlateStyle.h"
#include "DialogueAssetEditor/DialogueEditorRichTextBlock.h"
#include "Components/RichTextBlockDecorator.h"
#include "DialogueAssetEditor/AssetEditor_Dialogue.h"
#include "DialogueEditor.h"
#include "RenderingThread.h"

UDialogueEditorSettings::UDialogueEditorSettings()
{
	AutoLayoutStrategy = EAutoLayoutStrategy_DE::Tree;

	bFirstPassOnly = false;

	bRandomInit = false;

	OptimalDistance = 100.f;

	MaxIteration = 50;

	InitTemperature = 10.f;

	CoolDownRate = 10.f;

}

UDialogueEditorSettings::~UDialogueEditorSettings()
{

}



void UDialogueEditorSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);


}


//////////////////////////// Rich Text Style에 관련된 것 들 ///////////////////////////////
void UDialogueEditorSettings::PostLoad()
{
	Super::PostLoad();

}





template< class ObjectType >
struct FDialogueDeferredDeletor : public FDeferredCleanupInterface
{
public:
	FDialogueDeferredDeletor(ObjectType* InInnerObjectToDelete)
		: InnerObjectToDelete(InInnerObjectToDelete)
	{
	}

	virtual ~FDialogueDeferredDeletor()
	{
		delete InnerObjectToDelete;
	}

private:
	ObjectType* InnerObjectToDelete;
};
