#pragma once
#include "RudyPCH.h"

#include "Runtime/CoreFramework/Actor.h"
#include "Runtime/Components/Render/PrimitiveComponent.h"

namespace Rudy
{
class AStaticMeshActor : public AActor
{
public:
    ~AStaticMeshActor();
    AStaticMeshActor();

    void SetMaterial(SharedPtr<Material> mat);
    void SetShader(SharedPtr<Shader> shader);

    void BeginPlay() override;
    void EndPlay() override;
    void Tick(float DeltaTime) override;

public:
    SharedPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;
};

} // namespace Rudy