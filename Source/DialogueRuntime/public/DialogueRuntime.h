// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(DialogueRuntime, Log, All);

#define CUSTOMLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define RADLOG(VER, FMT, ...) UE_LOG(DialogueRuntime, VER, TEXT("%s %s"), *CUSTOMLOG_CALLINFO, *FString::Printf(FMT, ##__VA_ARGS__) )
#define LOG_INFO(FMT, ...) UE_LOG(DialogueRuntime, Display, TEXT("%s %s"), *CUSTOMLOG_CALLINFO, *FString::Printf(FMT, ##__VA_ARGS__) )
#define LOG_WARNING(FMT, ...) UE_LOG(DialogueRuntime, Warning, TEXT("%s %s"), *CUSTOMLOG_CALLINFO, *FString::Printf(FMT, ##__VA_ARGS__) )
#define LOG_ERROR(FMT, ...) UE_LOG(DialogueRuntime, Error, TEXT("%s %s"), *CUSTOMLOG_CALLINFO, *FString::Printf(FMT, ##__VA_ARGS__) )

class IDialogueRuntime : public IModuleInterface
{
public:

	static IDialogueRuntime& Get()
	{
		return FModuleManager::LoadModuleChecked<IDialogueRuntime>("DialogueRuntime");
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("DialogueRuntime");
	}
};

class FDialogueRuntime : public IDialogueRuntime
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};






