#pragma once
#include "Rudy/Renderer/Mesh.h" 

 
#include <glad/glad.h>
 
namespace Rudy
{
	class OpenGLMesh : public Mesh
	{
    public:
        ~OpenGLMesh() {
            glDeleteBuffers(1, &m_vertexBufferTarget); 
            glDeleteBuffers(1, &m_indexBufferTarget);
            glDeleteVertexArrays(1, &m_vertexArrayTarget);
         
         }
         OpenGLMesh() = default;

         virtual void SetupVertices() override;
         virtual void LoadToGPU() override;

        virtual void Bind()  override {
            glBindVertexArray(m_vertexArrayTarget);
            //RD_CORE_INFO("OpenGLMesh: VAO:{0} is bind", m_VertexArray);
        }

		virtual void Unbind() override { 
            glBindVertexArray(0); } 

        virtual uint32_t GetVertexArray() override 
		{return m_vertexArrayTarget;}

		virtual uint32_t GetIndexCount() override
		{return  indices.size();} 
  


	};


}  

