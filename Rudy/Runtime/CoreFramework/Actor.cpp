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

void AActor::Tick(float DeltaTime)
{
    for (auto& component : m_components)
    {
        component->TickComponent(DeltaTime);
    }
}

void AActor::BeginPlay()
{
    //   for (auto& component : m_components)
    //   {
    //	component->BeginPlay();
    //}
}

void AActor::EndPlay()
{
    //   for (auto& component : m_components)
    //   {
    //	component->EndPlay();
    //}
}

} // namespace Rudy