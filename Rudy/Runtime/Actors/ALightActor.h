#pragma once

#include "RudyPCH.h"

#include "Runtime/CoreFramework/Actor.h"
#include "Runtime/Components/Render/LightComponent.h"

namespace Rudy
{
class ASunlightActor : public AActor
{
public:
    ~ASunlightActor() = default;
    ASunlightActor();

    void Tick(float DeltaTime) override;
    void BeginPlay() override;
    void EndPlay() override;

public:
    SharedPtr<USunlightComponent> SunlightComponent = nullptr;
};
} // namespace Rudy