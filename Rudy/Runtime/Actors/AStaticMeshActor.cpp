#include "RudyPCH.h"

#include "AStaticMeshActor.h"

namespace Rudy
{

AStaticMeshActor::AStaticMeshActor()
{
    StaticMeshComponent = CreateComponentAsSubObject<UStaticMeshComponent>();
    RootComponent       = StaticMeshComponent;
}

AStaticMeshActor::~AStaticMeshActor()
{
    RD_CORE_WARN("AStaticMeshActor: {0} destroyed", Name);
}

void AStaticMeshActor::BeginPlay()
{
    AActor::BeginPlay();
}

void AStaticMeshActor::EndPlay()
{
    AActor::EndPlay();
}

void AStaticMeshActor::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
}

void AStaticMeshActor::SetMaterial(SharedPtr<Material> mat)
{
    StaticMeshComponent->SetMaterial(mat);
}

void AStaticMeshActor::SetShader(SharedPtr<Shader> shader)
{
    StaticMeshComponent->SetShader(shader);
}

} // namespace Rudy