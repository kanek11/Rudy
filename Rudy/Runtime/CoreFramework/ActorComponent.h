#pragma once
#include "RudyPCH.h"

#include "Object.h"

namespace Rudy
{

// forward declaration
class AActor;

//<<abstract>>
class UActorComponent : public UObject
{
public:
    virtual ~UActorComponent() = default;
    UActorComponent()          = default;

public:
    virtual void TickComponent(float DeltaTime) = 0;
    // void InitializeComponent();
    // void UninitializeComponent();

public:
    void RegisterOwner(const SharedPtr<AActor>& owner);

private:
    WeakPtr<AActor> m_owner; // weak reference to the owner, can't be null
    // bool              m_isRegistered = false;
};

// factory function for derived classes
// decision: register is separated from the factory function
template <typename T>
SharedPtr<T> CreateActorComponent()
{
    // abstract class cannot be instantiated
    static_assert(std::is_base_of<UActorComponent, T>::value, "T must be derived from UActorComponent");

    return CreateShared<T>();
}

} // namespace Rudy