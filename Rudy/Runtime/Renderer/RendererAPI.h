#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

// me:
// usecase:
// abstract class wrap 3D API commands ;
//
// static API type as a state;
// to be instantiated based on the API type;

// cherno define it using RenderCommand class to wrap as static functions
// i find it not necessary.

namespace Rudy
{

//<<interface>>
class RendererAPI
{
public:
    enum class API
    {
        None = 0,
        OpenGL,
        Vulkan,
        DirectX,
    };

public:
    virtual ~RendererAPI() = default;
    static SharedPtr<RendererAPI> Create();

    virtual void Init()                                                               = 0;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void SetClearColor(const glm::vec4& color)                                = 0;
    virtual void SetClearColor(float r, float g, float b, float a)                    = 0;
    virtual void Clear()                                                              = 0;

    virtual void DrawIndexed(MeshTopology topo, uint32_t indexCount)                                  = 0;
    virtual void DrawIndexedInstanced(MeshTopology topo, uint32_t indexCount, uint32_t instanceCount) = 0;
    virtual void DrawIndexedIndirect(MeshTopology topo)                                               = 0;

    virtual void DrawArrays(MeshTopology topo, uint32_t vertexCount)                                  = 0;
    virtual void DrawArraysInstanced(MeshTopology topo, uint32_t vertexCount, uint32_t instanceCount) = 0;
    virtual void DrawArraysIndirect(MeshTopology topo)                                                = 0;

    // virtual void SetLineWidth(float width) = 0;

    static API  GetAPI() { return s_API; }
    static void SetAPI(API api) { s_API = api; }

private:
    static API s_API;
};

} // namespace Rudy
