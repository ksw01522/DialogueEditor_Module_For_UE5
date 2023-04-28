#include "DialogueAssetEditor/Settings_DialogueEditor.h"

#include "Styling/SlateStyle.h"
#include "DialogueAssetEditor/DialogueEditorRichTextBlock.h"
#include "Components/RichTextBlockDecorator.h"
#include "DialogueAssetEditor/AssetEditor_Dialogue.h"
#include "DialogueEditor.h"
#include "RenderingThread.h"

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



void UDialogueEditorSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == "DialogueTextStyleSet") {
		OnChangedDialogueTextStyleSet();
		SaveConfig();
	}
	else if (PropertyChangedEvent.GetPropertyName() == "DecoratorClasses") {
		OnChangedDialogueTextDecorators();
		SaveConfig();
	}
}


//////////////////////////// Rich Text Style에 관련된 것 들 ///////////////////////////////
void UDialogueEditorSettings::PostLoad()
{
	Super::PostLoad();

	CheckRichTextBlock();
}

void UDialogueEditorSettings::OnChangedDialogueTextStyleSet()
{
	if (!CheckRichTextBlock()) return;

	RichTextBlock->SetTextStyleSet(DialogueTextStyleSet.Get());
	MakeStyleInstance();
	AssetEditor->OnChangedDialogueTextStyle();
}

void UDialogueEditorSettings::OnChangedDialogueTextDecorators()
{
	if (!CheckRichTextBlock()) return;

	RichTextBlock->SetDecorators(DecoratorClasses);
	AssetEditor->OnChangedDialogueTextStyle();
}

bool UDialogueEditorSettings::CheckRichTextBlock()
{
	if(!IsValid(RichTextBlock)){
		RichTextBlock = NewObject<UDialogueEditorRichTextBlock>(this, FName(TEXT("DialogueEditorRichTextBlock") ));
		if (IsValid(RichTextBlock)) {
			if (!DecoratorClasses.IsEmpty()) { OnChangedDialogueTextDecorators(); }
			if (DialogueTextStyleSet) { OnChangedDialogueTextStyleSet(); }
		}
	}

	return IsValid(RichTextBlock);
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
template< class ObjectType >
FORCEINLINE TSharedPtr< ObjectType > MakeDialogueShareableDeferredCleanup(ObjectType* InObject)
{
	return MakeShareable(InObject, [](ObjectType* ObjectToDelete) { BeginCleanup(new FDialogueDeferredDeletor<ObjectType>(ObjectToDelete)); });
}

void UDialogueEditorSettings::MakeStyleInstance()
{
	StyleInstance = MakeDialogueShareableDeferredCleanup(new FSlateStyleSet(TEXT("RichTextStyle")));

	if (DialogueTextStyleSet && DialogueTextStyleSet->GetRowStruct()->IsChildOf(FRichTextStyleRow::StaticStruct()))
	{
		for (const auto& Entry : DialogueTextStyleSet->GetRowMap())
		{
			FName SubStyleName = Entry.Key;
			FRichTextStyleRow* RichTextStyle = (FRichTextStyleRow*)Entry.Value;

			if (SubStyleName == FName(TEXT("Default")))
			{
				DialogueDefaultTextStyle = RichTextStyle->TextStyle;
			}

			StyleInstance->Set(SubStyleName, RichTextStyle->TextStyle);
		}
	}
}

void UDialogueEditorSettings::GetDialogueTextDecoratorInstances(TArray< TSharedRef< ITextDecorator > >& OutDecorators) const
{
	const TArray<TObjectPtr<URichTextBlockDecorator>> Decos = RichTextBlock->GetDialogueTextBlockDecoratorinstances();

	for (URichTextBlockDecorator* Decorator : Decos)
	{
		if (Decorator)
		{
			TSharedPtr<ITextDecorator> TextDecorator = Decorator->CreateDecorator(RichTextBlock);
			if (TextDecorator.IsValid())
			{
				OutDecorators.Add(TextDecorator.ToSharedRef());
			}
		}
	}
}