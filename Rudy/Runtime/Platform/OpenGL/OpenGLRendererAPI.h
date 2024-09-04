#pragma once
// cpp
#include "RudyPCH.h"

#include "Runtime/Renderer/RendererAPI.h"

namespace Rudy
{

class OpenGLRendererAPI : public RendererAPI
{
public:
    virtual void Init() override;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    virtual void SetClearColor(const glm::vec4& color) override;
    virtual void SetClearColor(float r, float g, float b, float a) override;
    virtual void Clear() override;

    virtual void DrawIndexed(MeshTopology topo, uint32_t indexCount) override;
    virtual void DrawIndexedInstanced(MeshTopology topo, uint32_t indexCount, uint32_t instanceCount) override;
    virtual void DrawIndexedIndirect(MeshTopology topo) override;

    virtual void DrawArrays(MeshTopology topo, uint32_t vertexCount) override;
    virtual void DrawArraysInstanced(MeshTopology topo, uint32_t vertexCount, uint32_t instanceCount) override;
    virtual void DrawArraysIndirect(MeshTopology topo) override;

    // virtual void DrawLines(const SharedPtr<VertexArray>& vertexArray, uint32_t vertexCount) override;

    // virtual void SetLineWidth(float width) override;*/
};

} // namespace Rudy
