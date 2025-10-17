#include "Modules/ModuleManager.h"

class FUEcsTestsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FUEcsTestsModule, UEcsTests)
