#pragma once

#include <vector>
#include "Hazel/Renderer/Mesh.h"
 

namespace Hazel {


	class Sphere {
	public:
		Sphere() { Create(subdivision);}
		~Sphere() = default;

		void Create(unsigned int subdivision);
        void DrawSphere();

		std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;

        unsigned int VertexArray;

        unsigned int subdivision = 20;
	};



    void Sphere::DrawSphere()
    {
        glBindVertexArray(VertexArray);
        glDrawElements(GL_TRIANGLE_STRIP, Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    


	//static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	void Sphere::Create(unsigned int  subdivision)
	{
    
  
        //generate using spherical coordinate£»
        //divide the theta and phi step £»the sudivision;
        //const unsigned int subdivision = 200;
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
 
	}





}


