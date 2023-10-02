#pragma once
#include "EtherPCH.h"
#include <glad/glad.h>


#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Object.h"
#include "Hazel/Renderer/Renderer.h"
 
//for now, i want the pre-defined model to be self-contained. not relying on object class design.

//all public, free to set;

namespace Hazel {


	class Sphere {
	public:
        Sphere()  { CreateGeometry(subdivision);}
		~Sphere() = default;

		void CreateGeometry(unsigned int subdivision);
        void Draw();
          

        Transform  Transform; 
        Ref<Mesh>  Mesh;
        Ref<Material>  Material;
        //Ref<Shader>  Shader;  
		  
        unsigned int subdivision = 20;
	};



    void Sphere::Draw ()
    {
        //glBindVertexArray(m_Mesh->GetVertexArray());
        //glBindVertexArray(0); 

         Mesh->Bind(); 
         Material->Bind();

        // set uniforms for transforms
        // identity matrix for now;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, Transform.Scale);
        model = glm::translate(model, Transform.Position);
        glm::mat4 view = Renderer::GetMainCamera()->GetViewMatrix();  
        

        glm::mat4 projection = Renderer::GetMainCamera()->GetProjectionMatrix(); 


         Material->GetShader()->SetMat4("u_Model", model); 
         Material->GetShader()->SetMat4("u_View", view);
         Material->GetShader()->SetMat4("u_Projection", projection);  

        glDrawElements(GL_TRIANGLE_STRIP,  Mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);

        
         Mesh->Unbind();
         Material->Unbind();
    }
    


	//static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	void Sphere::CreateGeometry(unsigned int subdivision)
	{

         std::vector<Vertex> Vertices;
         std::vector<unsigned int> Indices; 
  
        //generate using spherical coordinate��
        //divide the theta and phi step ��the sudivision;
        //const unsigned int subdivision = 200;
        const unsigned int X_SEGMENTS = subdivision;
        const unsigned int Y_SEGMENTS = subdivision;
        const float PI = 3.14159265359f;

        //x =azimuth ;y=�� elevation
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {


                //normalized
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;

                //y being up
                float yPos = std::cos(ySegment * PI);
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);



                Vertex vertex;

                vertex.Position = glm::vec3(xPos, yPos, zPos);
                vertex.Normal = glm::vec3(xPos, yPos, zPos);
                vertex.TexCoords = glm::vec2(xSegment, ySegment);

                glm::vec3 Tangent = 
                glm::vec3(-2.0f * PI * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI),
                0,
                2.0f * PI * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI));
                
                vertex.Tangent = glm::normalize(Tangent); 
                vertex.Bitangent = glm::normalize(glm::cross(glm::vec3(xPos, yPos, zPos), Tangent));

                 
                Vertices.push_back(vertex); 

            }
        }

        //generate indices ; the pos was generated in order of columns; 
        //displace SEGMENTS+1 each time  ; generate 2 indices at a time
        //make opengl draw "strip"

        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {

            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                Indices.push_back(y * (X_SEGMENTS + 1) + x);
                Indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }

        }
         
        //generate mesh
         Mesh = Mesh::Create(Vertices, Indices);


 
	}

      



}

