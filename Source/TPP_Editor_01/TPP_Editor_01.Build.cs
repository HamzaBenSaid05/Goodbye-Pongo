using UnrealBuildTool;

public class TPP_Editor_01 : ModuleRules
{
    public TPP_Editor_01(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "PropertyEditor",
                "DetailCustomizations",
                "EditorStyle", 
                "TPP_01",
                "EditorSubsystem",
            }
        );
    }
}