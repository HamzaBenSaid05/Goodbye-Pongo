#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FTPP_Editor_01Module : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
