#include "RudyPCH.h"


#include "Mesh.h"
#include "Rudy/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLMesh.h"


namespace Rudy {

    Ref<Mesh> Mesh::Create()
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    RD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLMesh>();
        }

        RD_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

     




}
