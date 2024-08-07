// ver 2024.1.26
#include "RudyPCH.h"

#include "Rudy/GameFramework/Actor.h"
#include "Rudy/GameFramework/ActorComponent.h"

using namespace Rudy;

class TestComponent : public UActorComponent
{
public:
    virtual void TickComponent(float DeltaTime) override
    {
        std::cout << "TickTime: " << DeltaTime << std::endl;
    }
};
class TestActor : public AActor
{
public:
    virtual void Tick(float DeltaTime) override
    {
        std::cout << "MyActor::Tick" << std::endl;
    }

    virtual void BeginPlay() override
    {
        std::cout << "MyActor::BeginPlay" << std::endl;
    }

    virtual void EndPlay() override
    {
        std::cout << "MyActor::EndPlay" << std::endl;
    }

    SharedPtr<TestComponent> testSubObject = nullptr;

    TestActor()
    {
        this->testSubObject = CreateComponentAsSubObject<TestComponent>();
    }
};

int main()
{
    // unit test kinda thing

    // Actor
    {
        auto actor = CreateActor<TestActor>();

        auto actorComponent = CreateActorComponent<TestComponent>();

        // dynamic
        actor->AddComponent(actorComponent);
        actorComponent->RegisterOwner(actor);

        auto _component = actor->GetComponent<UActorComponent>();
        _component->TickComponent(0.1f);
    }
}
