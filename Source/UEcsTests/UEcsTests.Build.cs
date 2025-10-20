//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.
using UnrealBuildTool;

public class UEcsTests : ModuleRules
{
    public UEcsTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // This is a test/developer module
        Type = ModuleType.CPlusPlus;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "CQTest"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "UEcs",
        });

        // Tests generally run in editor context
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] {
                "UnrealEd"
            });
        }

        bEnableExceptions = false;
        bUseUnity = true;
    }
}
