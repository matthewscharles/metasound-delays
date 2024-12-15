using UnrealBuildTool;

public class MetasoundDelays : ModuleRules
{
    public MetasoundDelays(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        // ???
        // PublicIncludePaths.Clear();
        // PrivateIncludePaths.Clear();
        
        PublicIncludePaths.AddRange(
            new string[] {
                "MetasoundDelays/Public"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                "MetasoundDelays/Private",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "MetasoundEngine",
                "MetasoundStandardNodes",
                "MetasoundFrontend", 
                "MetasoundGraphCore"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "AudioExtensions",
                "SignalProcessing"
            }
        );
    }
}