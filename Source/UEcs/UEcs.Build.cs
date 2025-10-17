// Copyright
using UnrealBuildTool;
using System.IO;

public class UEcs : ModuleRules
{
    public UEcs(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Projects"
        });

        // Third-party EnTT (header-only) is included privately to avoid leaking into dependents.
        string PluginDir = ModuleDirectory; // .../Plugins/UEcs/Source/UEcs
        string RootDir = Path.GetFullPath(Path.Combine(PluginDir, "..", "..")); // .../Plugins/UEcs
        string ThirdPartyDir = Path.Combine(RootDir, "ThirdParty");

        // Common include locations for EnTT when vendored
        string[] CandidateIncludeDirs = new string[] {
            Path.Combine(ThirdPartyDir, "entt", "single_include"),
            Path.Combine(ThirdPartyDir, "entt", "include"),
            Path.Combine(ThirdPartyDir, "entt")
        };

        foreach (var inc in CandidateIncludeDirs)
        {
            if (Directory.Exists(inc))
            {
                // entt is used in public headers, so expose include dirs publicly
                PublicIncludePaths.Add(inc);
            }
        }

        bEnableExceptions = false;
        bUseUnity = true;
    }
}
