#pragma once

#include "CoreMinimal.h"
#include "DialogueAssetEditor/DialogueEditorRichTextBlock.h"
#include "Settings_DialogueEditor.generated.h"

UENUM(BlueprintType)
enum class EAutoLayoutStrategy : uint8
{
	Tree,
	ForceDirected,
};

UCLASS(Config = DialogueEditor)
class UDialogueEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	UDialogueEditorSettings();
	virtual ~UDialogueEditorSettings();

	class FAssetEditor_Dialogue* AssetEditor;

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
	float OptimalDistance;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	EAutoLayoutStrategy AutoLayoutStrategy;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	int32 MaxIteration;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	bool bFirstPassOnly;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	bool bRandomInit;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	float InitTemperature;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	float CoolDownRate;

public:
	virtual void PostLoad() override;

///////////////////// RichText Style에 관련된 것들 //////////////////
private:
	UPROPERTY(EditAnywhere, Category = "DialogueTextStyle", GlobalConfig, meta = (RequiredAssetDataTags = "RowStructure=/Script/UMG.RichTextStyleRow"))
	TSoftObjectPtr<UDataTable> DialogueTextStyleSet;

	UPROPERTY(EditAnywhere, Category = "DialogueTextStyle", GlobalConfig)
	TArray<TSubclassOf<URichTextBlockDecorator>> DecoratorClasses;


	UPROPERTY(Transient)
	TObjectPtr<UDialogueEditorRichTextBlock> RichTextBlock = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<URichTextBlockDecorator>> InstanceDecorators;
	TSharedPtr<class FSlateStyleSet> StyleInstance;

	UPROPERTY(Transient)
	FTextBlockStyle DialogueDefaultTextStyle;


	void OnChangedDialogueTextStyleSet();
	void OnChangedDialogueTextDecorators();

	bool CheckRichTextBlock();

	void MakeStyleInstance();

public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	TObjectPtr<UDialogueEditorRichTextBlock>  GetRichTextBlock() { return RichTextBlock; }

	UDataTable* GetDialogueTextStyleSet() const {return DialogueTextStyleSet.Get(); };

	const TSharedPtr<class FSlateStyleSet>& GetDialogueStyleSet(){return StyleInstance; }
	void GetDialogueTextDecoratorInstances(TArray< TSharedRef< class ITextDecorator > >& OutDecorators) const;
	const FTextBlockStyle& GetDialougeDefaultTextStyle() const {return DialogueDefaultTextStyle; }
};
