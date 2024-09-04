
#include "RudyPCH.h"

#include "ALightActor.h"

namespace Rudy
{

ASunlightActor::ASunlightActor() :
    AActor()
{
    SunlightComponent = CreateComponentAsSubObject<USunlightComponent>();
    RootComponent     = SunlightComponent;
}

void ASunlightActor::BeginPlay()
{
    AActor::BeginPlay();
}

void ASunlightActor::EndPlay()
{
    AActor::EndPlay();
}

void ASunlightActor::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
}

} // namespace Rudy
