#pragma once

#include "Modules/ModuleManager.h"

class FUEcsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
