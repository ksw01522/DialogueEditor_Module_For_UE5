// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SEdNode_DialogueNode.h"
#include "EdGraphNode_Dialogue_Basic.h"
/**
 * 
 */
 class UEdGraphNode_Dialogue_Basic;

class SEdNode_Dialogue_Basic : public SEdNode_DialogueNode
{
	typedef SEdNode_DialogueNode Super;
public:
	SLATE_BEGIN_ARGS(SEdNode_Dialogue_Basic) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphNode_Dialogue_Basic* InNode);

private:
	UEdGraphNode_Dialogue_Basic* EdNode_Basic;

protected:
	TSharedPtr<class SRichTextBlock> DialogueStringTextBlock = nullptr;
	TSharedPtr<STextBlock> OriginalDialogueStringTextBlock = nullptr;
	TSharedPtr<SButton> DialogueImportBTN = nullptr;

private:
	virtual FText GetDialogueString() const;
	virtual FText GetOriginalDialogueString() const;
	virtual EVisibility CanVisibleDialogue() const;
	virtual EVisibility CanVisibleOriginalDialogue() const;
	virtual EVisibility CanVisibleImportBTN() const;
	virtual FReply ImportDialogue();


//////////////////////////// Rich Text Block /////////////////////////////
	UPROPERTY(Transient)
	TObjectPtr<class URichTextBlock> RichTextBlock = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class URichTextBlockDecorator>> InstanceDecorators;
	TSharedPtr<class FSlateStyleSet> StyleInstance;
	UPROPERTY(Transient)
	FTextBlockStyle DialogueDefaultTextStyle;

	void GetDecoClasses(TArray<TSubclassOf<URichTextBlockDecorator>>& OutDecoClasses) const;

	void MakeStyleInstance();
	void MakeDecoInstance(TArray< TSharedRef<class ITextDecorator > >& OutDecorators);

/////////////////////////////////////////////////////////////////////////
protected:
	virtual TSharedPtr<SCompoundWidget> CreateNodeBody() override;

public:
	void UpdateRichTextStyle();

};
