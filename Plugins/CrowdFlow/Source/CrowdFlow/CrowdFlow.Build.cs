using UnrealBuildTool;

public class CrowdFlow : ModuleRules
{
	public CrowdFlow(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "AIModule" });

		PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine"	});
       

	}
}