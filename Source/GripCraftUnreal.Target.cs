// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GripCraftUnrealTarget : TargetRules
{
	public GripCraftUnrealTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("GripCraftUnreal");
	}
}
