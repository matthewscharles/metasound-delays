using UnrealBuildTool;

public class MetasoundEchoes : ModuleRules
{
    public MetasoundEchoes(ReadOnlyTargetRules Target) : base(Target)
    {

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