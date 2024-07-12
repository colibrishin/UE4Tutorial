// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MyProject: ModuleRules
{
	public MyProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "ProjectPrecompiled.h";
		MinFilesUsingPrecompiledHeaderOverride = 1;
		bUseUnity = false;
		CppStandard = CppStandardVersion.Default;

		PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"SlateCore",
			"UMG",
			"NavigationSystem",
			"AIModule",
			"GameplayTasks",
			"Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
        {
			"InputCore"
        });
	}
}
