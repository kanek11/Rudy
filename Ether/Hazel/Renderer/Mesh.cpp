

#include "Mesh.h"
#include "Hazel/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLMesh.h"

namespace Hazel {

	Scope<Mesh> Mesh::Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLMesh>(vertices, indices);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	} 

}