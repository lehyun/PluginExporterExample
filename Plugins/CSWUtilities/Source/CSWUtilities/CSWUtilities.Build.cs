// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CSWUtilities : ModuleRules
{
	public CSWUtilities(ReadOnlyTargetRules Target) : base(Target)
	{
		//Override UnrealEngineBuildTool to speedup compilation times
		MinFilesUsingPrecompiledHeaderOverride = 1;
		bFasterWithoutUnity = true;

		//Include What you need convention activation
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;

		//Engine dependencies
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "JsonUtilities" });

		//The path for the header files
		PublicIncludePaths.AddRange(new string[] { "CSWUtilities/Public" });

		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] { "CSWUtilities/Private" });
	}
}
