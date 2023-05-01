// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DialogueRuntime : ModuleRules
{
	public DialogueRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bLegacyPublicIncludePaths = false;
		ShadowVariableWarningLevel = WarningLevel.Error;

        PublicIncludePaths.AddRange(new string[]
        {
            "DialogueRuntime/public",
        });

        PrivateIncludePaths.AddRange(new string[]
		{
			"DialogueRuntime/public",
			"DialogueRuntime/private",
        });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine"});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"GameplayTags",
				"UMG",
                "RenderCore"
            });
	}
}
