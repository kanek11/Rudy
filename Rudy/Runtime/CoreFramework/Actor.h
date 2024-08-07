#pragma once
#include "RudyPCH.h"

#include "Object.h"
#include "ActorComponent.h"

// todo: actor can be placed in "level, world",
// the load/streaming of actors can be subtle;

// std::enable_shared_from_this is safer ;

namespace Rudy
{
// Actor can be placed in the game world;
//<<abstract>>
class AActor : public UObject, public std::enable_shared_from_this<AActor>
{
public:
    virtual ~AActor() = default;
    AActor()          = default;

public:
    // game loop
    virtual void Tick(float DeltaTime) = 0;
    virtual void BeginPlay()           = 0;
    virtual void EndPlay()             = 0;

public:
    // dynamic components
    void AddComponent(const SharedPtr<UActorComponent>& component);
    // void RemoveComponent();

    void RegisterAllComponents();

    template <typename T>
    SharedPtr<T> GetComponent() const // dynamic cast is a bit slow but safe
    {
        for (auto& component : m_components)
        {
            if (auto castedComponent = std::dynamic_pointer_cast<T>(component))
            {
                return castedComponent;
            }
        }
        return nullptr;
    }

    // factory for subobjects
    template <typename T>
    SharedPtr<T> CreateComponentAsSubObject()
    {
        auto _component = CreateActorComponent<T>();
        this->AddComponent(_component);

        return _component;
    }

private:
    std::vector<SharedPtr<UActorComponent>> m_components; // dynamic and subobjects
};

// factory function for derived classes
template <typename T>
SharedPtr<T> CreateActor()
{
    // abstract class cannot be instantiated
    static_assert(std::is_base_of<AActor, T>::value, "T must be derived from AActor");

    auto _actor = CreateShared<T>();

    _actor->RegisterAllComponents(); // register the actor to the components,  post-construction of the actor;

    return _actor;
}

} // namespace Rudy
