// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MP_Course : ModuleRules
{
	public MP_Course(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
