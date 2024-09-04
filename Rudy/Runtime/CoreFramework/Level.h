#pragma once
#include "RudyPCH.h"

#include "Actor.h"

namespace Rudy
{

class Level
{
public:
    ~Level() = default;
    Level()  = default;

public:
    void TickLevel()
    {
        // set uniform buffers ;  the binding point is the same for all shaders;
        m_main_camera->Tick();

        for (auto& _light : m_lights)
        {
            _light->Tick();
        }

        for (auto& _meshActor : m_meshActors)
        {
            _meshActor->Tick();
        }
    };

private:
    std::vector<SharedPtr<AMeshActor>> m_meshActors;
    SharedPtr<ACamera>                 m_main_camera;
    std::vector<SharedPtr<ALight>>     m_lights;
};

} // namespace Rudy

//  std::vector<SharedPtr<AActor>>     m_actors;