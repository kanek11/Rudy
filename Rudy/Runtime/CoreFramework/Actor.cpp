#pragma once
#include "RudyPCH.h"

#include "Actor.h"

namespace Rudy
{

void AActor::AddComponent(const SharedPtr<UActorComponent>& component)
{
    m_components.push_back(component);
}

void AActor::RegisterAllComponents()
{
    for (auto& component : m_components)
    {
        component->RegisterOwner(this->shared_from_this());
    }
}

} // namespace Rudy