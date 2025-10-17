#include "UEcsModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "FUEcsModule"

void FUEcsModule::StartupModule()
{
    // Optionally, verify that EnTT is present in ThirdParty. We don't error hard to keep the plugin usable without immediate vendoring.
#if !UE_BUILD_SHIPPING
    const FString PluginBase = IPluginManager::Get().FindPlugin(TEXT("UEcs"))->GetBaseDir();
    const FString EnTTPathA = FPaths::Combine(PluginBase, TEXT("ThirdParty"), TEXT("entt"), TEXT("single_include"), TEXT("entt"), TEXT("entt.hpp"));
    const FString EnTTPathB = FPaths::Combine(PluginBase, TEXT("ThirdParty"), TEXT("entt"), TEXT("include"), TEXT("entt"), TEXT("entt.hpp"));
    const bool bHasEnTT = FPaths::FileExists(EnTTPathA) || FPaths::FileExists(EnTTPathB);
    if (!bHasEnTT)
    {
        UE_LOG(LogTemp, Warning, TEXT("UEcs: EnTT headers not found under Plugins/UEcs/ThirdParty/entt. You can drop the official 'single_include/entt/entt.hpp' here."));
    }
#endif
}

void FUEcsModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUEcsModule, UEcs);
