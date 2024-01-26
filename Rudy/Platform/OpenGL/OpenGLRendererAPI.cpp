 
#include "RudyPCH.h"

#include "Rudy/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h" 
namespace Rudy
{
	namespace utils
	{
		GLenum MeshTopologyToGLMode(MeshTopology topo)
		{
			switch (topo)
			{
			case MeshTopology::TRIANGLES:
				return GL_TRIANGLES;
			case MeshTopology::LINES:
				return GL_LINES;
			case MeshTopology::POINTS:
				return GL_POINTS;
			case MeshTopology::STRIP:
				return GL_TRIANGLE_STRIP;
			case MeshTopology::QUADS:
				return GL_QUADS;
			}


			RD_CORE_ASSERT(false, "Unknown MeshTopology!");
			return 0;
		}

	}



	//void OpenGLRendererAPI::DrawElements(const Ref<Mesh> &mesh, const Ref<Material> &material, Transform transform)
	void OpenGLRendererAPI::DrawIndexed(MeshTopology topo, uint32_t indexCount)
	{ 
		//RD_PROFILE_FUNCTION(); 

		auto GL_mode = utils::MeshTopologyToGLMode(topo);
		 
		glDrawElements(GL_mode, indexCount, GL_UNSIGNED_INT, 0);
	}

	void OpenGLRendererAPI::DrawIndexedInstanced(MeshTopology topo, uint32_t indexCount, uint32_t instanceCount)
	{
		//RD_PROFILE_FUNCTION(); 

	    auto GL_mode = utils::MeshTopologyToGLMode(topo);
	    glDrawElementsInstanced(GL_mode, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
	   
	} 


	void OpenGLRendererAPI::DrawArrays(MeshTopology topo,uint32_t vertexCount)
	{
		//RD_PROFILE_FUNCTION(); 

	     auto GL_mode = utils::MeshTopologyToGLMode(topo);
		 glDrawArrays(GL_mode, 0, vertexCount); 
	}

	void OpenGLRendererAPI::DrawArraysInstanced(MeshTopology topo, uint32_t vertexCount, uint32_t instanceCount)
	{
		//RD_PROFILE_FUNCTION(); 

		auto GL_mode = utils::MeshTopologyToGLMode(topo);
		glDrawArraysInstanced(GL_mode, 0, vertexCount, instanceCount);
	}

	void OpenGLRendererAPI::DrawArraysIndirect(MeshTopology topo)
	{
		//RD_PROFILE_FUNCTION(); 

		auto GL_mode = utils::MeshTopologyToGLMode(topo);
		glDrawArraysIndirect(GL_mode, 0);

	}


   void OpenGLRendererAPI::DrawIndexedIndirect(MeshTopology topo)
	{
		//RD_PROFILE_FUNCTION();  
		auto GL_mode = utils::MeshTopologyToGLMode(topo);
		glDrawElementsIndirect(GL_mode, GL_ELEMENT_ARRAY_BUFFER, 0);

	}
	 


	

	void OpenGLRendererAPI::Init()
	{
		//RD_PROFILE_FUNCTION();  
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4 &color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	//{
	//	vertexArray->Bind();
	//	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	//	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	// }

	// void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	//{
	//	vertexArray->Bind();
	//	glDrawArrays(GL_LINES, 0, vertexCount);
	// }

	// void OpenGLRendererAPI::SetLineWidth(float width)
	//{
	//	glLineWidth(width);
	// }

}