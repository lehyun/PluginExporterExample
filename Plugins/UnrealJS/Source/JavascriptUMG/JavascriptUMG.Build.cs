using UnrealBuildTool;

public class JavascriptUMG : ModuleRules
{
	public JavascriptUMG(ReadOnlyTargetRules Target) : base(Target)
	{
		//Override UnrealEngineBuildTool to speedup compilation times
		MinFilesUsingPrecompiledHeaderOverride = 1;
		bFasterWithoutUnity = true;

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;

		PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "Slate",
			"SlateCore",
            "ShaderCore",
			"RenderCore",
			"RHI", 
            "UMG",
            "V8"
        });        
	}
}
