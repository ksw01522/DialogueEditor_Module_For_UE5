#include "DialogueAssetEditor/Settings_DialogueEditor.h"

UDialogueEditorSettings::UDialogueEditorSettings()
{
	AutoLayoutStrategy = EAutoLayoutStrategy::Tree;

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

