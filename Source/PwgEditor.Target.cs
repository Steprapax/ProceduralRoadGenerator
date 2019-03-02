using UnrealBuildTool;
using System.Collections.Generic;

public class PwgEditorTarget : TargetRules
{
    public PwgEditorTarget(TargetInfo Target) : base(Target)

    {
		Type = TargetType.Editor;
        ExtraModuleNames.Add("Pwg");
    }
}
