#pragma once
#include "EtherPCH.h"
#include <glad/glad.h>


#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Object.h"
#include "Hazel/Renderer/Renderer.h"
 
//for now, i want the pre-defined model to be self-contained. not relying on object class design.

//all public, free to set;

namespace Hazel {


	class Sphere: public MeshObject {

	public:
        Sphere(unsigned int subdivision)  
            :m_subdivision(subdivision)
        { CreateGeometry(subdivision);}

		~Sphere() = default;

		void CreateGeometry(unsigned int subdivision);

        void Draw() override;
          
 
        //Ref<Mesh>  Mesh;
        //Ref<Material>  Material; 
		  
        uint32_t m_subdivision = 20;
	};



    void Sphere::Draw ()
    {
        //glBindVertexArray(m_Mesh->GetVertexArray());
        //glBindVertexArray(0); 

        m_Mesh->Bind(); 
        m_Material->Bind();

        // set uniforms for transforms
        // identity matrix for now;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, Transform.Scale);
        model = glm::translate(model, Transform.Position); 

        glm::mat4 projection_view = Renderer::GetMainCamera()->GetProjectionViewMatrix(); 


        m_Material->GetShader()->SetMat4("u_Model", model);  
        m_Material->GetShader()->SetMat4("u_ProjectionView", projection_view);

        glDrawElements(GL_TRIANGLE_STRIP,  m_Mesh->GetIndexCount(),  GL_UNSIGNED_INT, 0);

        
        m_Mesh->Unbind();
        m_Material->Unbind();
    }
    


	//static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    //based on spherical coordinate
    //opengl y is up,  elevation or theta,  vary from 0 to pi
    //xz plane is azimuth or phi, vary from 0 to 2pi

    //subdivision say 4 , means 4 facets; 
    //say theta varys   [0,..4]* pi/4
	void Sphere::CreateGeometry(unsigned int subdivision)
	{

         std::vector<Vertex> Vertices;
         std::vector<unsigned int> Indices; 


        const unsigned int X_SEGMENTS = subdivision;
        const unsigned int Y_SEGMENTS = subdivision;
        const float PI = 3.14159265359f;

        //x =azimuth ;y=¦È elevation
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            { 

                //normalized
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPhi = xSegment * 2.0f * PI;
                float yTheta = ySegment * PI ;

                //y being up
                float yPos = std::cos(yTheta);
                float xPos = std::cos(xPhi) * std::sin(yTheta);
                float zPos = std::sin(xPhi) * std::sin(yTheta);


                Vertex vertex;

                vertex.Position = glm::vec3(xPos, yPos, zPos);
                vertex.Normal = glm::vec3(xPos, yPos, zPos);
                vertex.TexCoords = glm::vec2(xSegment, ySegment);

  
                //tangent ,always on xz plane
                //as derivative to normal , dN/dphi = (-sin(phi), 0, cos(phi)) * sin(theta)

                //note at poles, sintheta = 0, tangent is undefined;
                if (yTheta  == 0)
                {
					vertex.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
					//vertex.Bitangent = glm::vec3(0.0f, 0.0f, 1.0f); 
				}
                else if (yTheta == PI)
                {
                    vertex.Tangent = glm::vec3(-1.0f, 0.0f, 0.0f);
                }
                else
                {
                    glm::vec3 Tangent =
                        glm::vec3(-std::sin(xPhi) * std::sin(yTheta),
                            0,
                            std::cos(xPhi) * std::sin(yTheta));

                    vertex.Tangent = glm::normalize(Tangent);

                    //optional bitangent
                    //vertex.Bitangent = glm::normalize(glm::cross(glm::vec3(xPos, yPos, zPos), Tangent));
                }
               
                 
                Vertices.push_back(vertex); 

            }
        }

        //generate indices ; 
        // draw "strip" mode ; +2 indices for each new triangle

        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        { 
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                Indices.push_back(y * (X_SEGMENTS + 1) + x);
                Indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }

        }
         
        //generate mesh
         m_Mesh = Mesh::Create(Vertices, Indices);


 
	}

      



}


