#include "EtherPCH.h"


#include "Mesh.h"
#include "Hazel/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLMesh.h"


namespace Hazel {

    Ref<Mesh> Mesh::Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLMesh>(vertices, indices);
        }

        HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

}
