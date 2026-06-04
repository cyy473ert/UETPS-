// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Demo_cyy : ModuleRules
{
	public Demo_cyy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateIncludePaths.AddRange(new string[] {
			ModuleDirectory,
			ModuleDirectory + "/Gameplay/Player",
			ModuleDirectory + "/Types",
			ModuleDirectory + "/Gameplay/GameModes",
			ModuleDirectory + "/Save",
			ModuleDirectory + "/UI",
			ModuleDirectory + "/Weapons",
			ModuleDirectory + "/DataAssets",
			ModuleDirectory + "/Components",
			ModuleDirectory + "/Combat",
			ModuleDirectory + "/AI/Enemy",
			ModuleDirectory + "/Interaction",
			ModuleDirectory + "/Gameplay/Interactables"

		});

        PublicDependencyModuleNames.AddRange(new string[]
		{ "Core", 
			"CoreUObject", 
			"Engine",
			"InputCore", 
			"EnhancedInput",
            "UMG",          
            "Slate",        
            "SlateCore",
            "AIModule",
			"NavigationSystem"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

