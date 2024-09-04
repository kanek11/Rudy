#pragma once
#include "RudyPCH.h"

#include "Runtime/CoreFramework/Actor.h"
#include "Runtime/Components/Render/CameraComponent.h"

namespace Rudy
{
class ACameraActor : public AActor
{
public:
    ~ACameraActor() = default;
    ACameraActor();

    void BeginPlay() override;
    void EndPlay() override;
    void Tick(float DeltaTime) override;

public:
    SharedPtr<UCameraComponent> CameraComponent = nullptr;
};

} // namespace Rudy