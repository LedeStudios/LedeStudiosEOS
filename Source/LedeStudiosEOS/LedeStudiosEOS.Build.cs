// Copyright Lede Studios. All Rights Reserved.

using UnrealBuildTool;

public class LedeStudiosEOS : ModuleRules
{
	public LedeStudiosEOS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", 
			"OnlineSubsystemUtils", "DeveloperSettings", "VoiceChat" });

		PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem" });

		PrivateIncludePaths.AddRange(new string[] { "LedeStudiosEOS/" });
		
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
