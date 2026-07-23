// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TPP_01 : ModuleRules
{
	public TPP_01(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",         
				"CoreUObject",
				"Engine",
				"EnhancedInput",
				"NavigationSystem",
				"AIModule",
				"UMG",
				"GameplayTags",
				"SlateCore", 
				"Slate",
				"DeveloperSettings", 
				"CommonUI",
				"CommonInput",
				"Niagara",
				"InputCore",
				"MotionWarping", 
				"LevelSequence",
				"MovieScene",
				"RenderCore",
			}
		);

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
