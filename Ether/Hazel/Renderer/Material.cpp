
#include "Ether.h"
#include "Material.h"

#include "Hazel/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLMaterial.h" 


namespace Hazel

{

	Ref<Material> Material::Create(Ref<Shader> shader, WorkflowMode workflowMode)
	{
		 
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); 
			return nullptr;
		case RendererAPI::API::OpenGL:  
			return CreateRef<OpenGLMaterial>(shader , workflowMode);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;


	}


}