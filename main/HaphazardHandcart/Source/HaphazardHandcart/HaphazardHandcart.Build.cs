// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class HaphazardHandcart : ModuleRules
{
	public HaphazardHandcart(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Paper2D", "PhysXVehicles", "HeadMountedDisplay", "GameplayTasks", "AIModule", "Niagara", "ApexDestruction" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Niagara" });

        PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");
	}
}
