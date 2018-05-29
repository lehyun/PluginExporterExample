using UnrealBuildTool;

public class JavascriptHttp : ModuleRules
{
	public JavascriptHttp(ReadOnlyTargetRules Target) : base(Target)
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
            "V8",
            "Http"
        });

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }
	}
}
