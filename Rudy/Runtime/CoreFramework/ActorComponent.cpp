#include "RudyPCH.h"

#include "ActorComponent.h"

namespace Rudy
{
UActorComponent::UActorComponent() :
    UObject()
{
}

void UActorComponent::TickComponent(float DeltaTime)
{
    // do nothing for now;
}

void UActorComponent::RegisterOwner(const SharedPtr<AActor>& owner)
{
    this->m_owner = owner;
}

WeakPtr<AActor> UActorComponent::GetOwner() const
{
    return this->m_owner;
}

} // namespace Rudy