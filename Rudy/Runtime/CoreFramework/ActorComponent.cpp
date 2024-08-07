#include "RudyPCH.h"

#include "ActorComponent.h"

namespace Rudy
{

void UActorComponent::RegisterOwner(const SharedPtr<AActor>& owner)
{
    this->m_owner = owner;
}

} // namespace Rudy