using UnrealBuildTool;

public class CrowdFlowEditor : ModuleRules
{
	public CrowdFlowEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core" });

		PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine", "Slate", "SlateCore", "InputCore" });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] {
            "CrowdFlow",
            "EditorStyle",
            "EditorFramework",
            "UnrealEd",
            "LevelEditor",
            "InteractiveToolsFramework",
            "EditorInteractiveToolsFramework"
             });
        }
       

	}
}