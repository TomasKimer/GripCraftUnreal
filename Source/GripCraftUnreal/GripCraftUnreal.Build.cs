// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GripCraftUnreal : ModuleRules
{
	public GripCraftUnreal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "ProceduralMeshComponent" });
	}
}
