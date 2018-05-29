// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class JavascriptGraphEditor : ModuleRules
	{
		public JavascriptGraphEditor(ReadOnlyTargetRules Target) 
            : base(Target)
		{
			//Override UnrealEngineBuildTool to speedup compilation times
			MinFilesUsingPrecompiledHeaderOverride = 1;
			bFasterWithoutUnity = true;

			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
			bEnforceIWYU = true;

			PrivateIncludePaths.AddRange(
				new string[] {
                    "JavascriptGraphEditor/Private",
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
                    "Engine",
                    "UnrealEd",
                    "UMG"
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
                    "AssetTools",
                    "Slate",
                    "SlateCore",
                    "GraphEditor",
                    "EditorStyle",
                    "JavascriptUMG",
                    "KismetWidgets",
                }
				);
		}
	}
}