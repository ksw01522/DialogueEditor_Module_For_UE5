// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateTypes.h"
#include "Framework/Text/TextLayout.h"
#include "Framework/Text/ISlateRun.h"
#include "Framework/Text/ITextDecorator.h"
#include "SRichTextBlockDecorator.generated.h"

/**
 * 
 */
class ISlateStyle;
class USRichTextBlockDecorator;
class SRichTextBlock;

class DIALOGUERUNTIME_API FSRichTextDecorator : public ITextDecorator
{
public:
	FSRichTextDecorator(const FTextBlockStyle& Style) : TextStyle(Style)
	{}

	virtual ~FSRichTextDecorator() {}

	/** Override this function to specify which types of tags are handled by this decorator */
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		return false;
	}

	virtual TSharedRef<ISlateRun> Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style) override final;

protected:
	/** Override this function if you want to create a unique widget like an image */
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& DefaultTextStyle) const;

	/** Override this function if you want to dynamically generate text, optionally changing the style. InOutString will start as the content between tags */
	virtual void CreateDecoratorText(const FTextRunInfo& RunInfo, FTextBlockStyle& InOutTextStyle, FString& InOutString) const;

	FTextBlockStyle TextStyle;
};

UCLASS(Abstract, Blueprintable)
class DIALOGUERUNTIME_API USRichTextBlockDecorator : public UObject
{
	GENERATED_BODY()
	
public:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(const FTextBlockStyle& Style);

};
