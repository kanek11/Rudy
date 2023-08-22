#pragma once
#include "Hazel/Renderer/Mesh.h" 

 
#include <glad/glad.h>
 
namespace Hazel
{
	class OpenGLMesh : public Mesh
	{
    public:
        OpenGLMesh() = default;
        OpenGLMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);   
        ~OpenGLMesh() = default ;
 

        //pass data, setup vao that is used by draw command.
        virtual void SetupMesh() override; 

        virtual void Bind()  override {
            glBindVertexArray(m_VertexArray);
            //HZ_CORE_INFO("OpenGLMesh: VAO:{0} is bind", m_VertexArray);
        }

		virtual void Unbind() override { 
            glBindVertexArray(0); }
        //virtual void DrawMesh() override;

        virtual unsigned int GetVertexAarry() override 
		{return m_VertexArray;}

		virtual unsigned int GetIndexCount() override
		{return m_Indices.size();}

    private:
        unsigned int m_VertexBuffer, m_IndexBuffer, m_VertexArray;
        std::vector<Vertex>       m_Vertices;
        std::vector<unsigned int>  m_Indices; 

	};


}  

