// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BlackWK : ModuleRules
{
	public BlackWK(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject", 
			"Engine",
			"InputCore",
			"EnhancedInput",
			
			"UMG",
			
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"GameplayMessageRuntime",
			
			"AIModule",
			"NetCore",
			
			"MotionWarping"
		});
	}
}