using UnrealBuildTool;
using System.Collections.Generic;

public class PwgTarget : TargetRules
{
	public PwgTarget(TargetInfo Target): base (Target)
	{
		Type = TargetType.Game;
        ExtraModuleNames.Add("Pwg");
    }
}
