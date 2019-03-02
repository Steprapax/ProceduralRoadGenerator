using UnrealBuildTool;

public class Pwg : ModuleRules
{
#if WITH_FORWARDED_MODULE_RULES_CTOR
    public Pwg(ReadOnlyTargetRules Target) : base(Target)
#else
    public Pwg(TargetInfo Target)
#endif
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay","ProceduralMeshComponent", "RuntimeMeshComponent", "ShaderCore", "RenderCore", "RHI"});
    }
}
