// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GripCraftUnrealEditorTarget : TargetRules
{
	public GripCraftUnrealEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("GripCraftUnreal");
	}
}
