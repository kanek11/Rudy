
#include "RudyPCH.h"

#include "ACameraActor.h"

namespace Rudy
{

ACameraActor::ACameraActor()
{
    this->CameraComponent = CreateComponentAsSubObject<UCameraComponent>();
    RootComponent         = CameraComponent;
}

void ACameraActor::BeginPlay()
{
    AActor::BeginPlay();
}

void ACameraActor::EndPlay()
{
    AActor::EndPlay();
}

void ACameraActor::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
}

} // namespace Rudy