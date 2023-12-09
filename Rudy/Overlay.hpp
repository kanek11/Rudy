#pragma once

#include "RudyPCH.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"

#include "Rudy/Renderer/Renderer.h"



namespace Rudy
{

     
	class WorldGrid {

	public:
		WorldGrid(int size) { CreateGeometry(size); }
		~WorldGrid() = default;

		void CreateGeometry(int size);
		void Draw(); 

		uint32_t gridVAO, gridVBO;

		//address geometry differently
		std::vector<float> gridVertices; // 10 vertical and 10 horizontal lines, each requiring 4 vertices

		//Transform  Transform;

		Ref<Material>  material;

	};



    //only geometry.  no material, no shader;
    //the grid is implemented by drawing lines;   

    //use two vertices to specify a line.  
    // 
    //the grid is centered at origin;  the size is the number of grids in each direction;  
    //i.e.  size = 10,  then the grid is 21x21;  vary from -10 to 10;   
    // 
    //the total number of vertices must be even.  vertices must be in pairs.


    void WorldGrid::CreateGeometry(int size)
    {
        RD_CORE_INFO("WorldGrid::CreateGeometry");


        //the grid is centered at origin;  the size is the number of grids in each direction;  i.e.  size = 10,  then the grid is 20x20;
        //the grid is in xz plane;  y = 0;

        for (int i = -size; i <= size; i++)
        {
            //horizontal lines,  along x axis;  vary z;
            float hx = (float)size;
            float hz = (float)i; 

            //left  
            gridVertices.push_back(-hx);
            gridVertices.push_back(0.0);
            gridVertices.push_back(hz);
            // right
            gridVertices.push_back(hx);
            gridVertices.push_back(0.0);
            gridVertices.push_back(hz);

            //vertical
            float vx = (float)i;
            float vz = (float)size;

            gridVertices.push_back(vx);
            gridVertices.push_back(0.0);
            gridVertices.push_back(-vz);
            // right
            gridVertices.push_back(vx);
            gridVertices.push_back(0.0);
            gridVertices.push_back(vz);

        }

        //generate mesh
        //Mesh = Mesh::Create(Vertices, Indices);


        glGenVertexArrays(1, &gridVAO);
        glGenBuffers(1, &gridVBO);

        glBindVertexArray(gridVAO);

        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        RD_CORE_INFO("worldgrid: data passed, size:{0}", gridVertices.size());

        // Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


    }

     


    void WorldGrid::Draw()
    {

        material->Bind();
        glBindVertexArray(gridVAO);


        glm::mat4 model = glm::mat4(1.0f);   
        glm::mat4 projection_view = Renderer::GetMainCamera()->GetProjectionViewMatrix(); 

        material->GetShader()->SetMat4("u_Model", model);
        material->GetShader()->SetMat4("u_ProjectionView", projection_view); 


        glDrawArrays(GL_LINES, 0, gridVertices.size()/3);  // 3 because each vertex has 3 floats.
 
        glBindVertexArray(0); 
        material->Unbind();

    }




    //x red , y green, z blue ; 
    class Navigation {

    public:
        Navigation() { CreateGeometry(); }
        ~Navigation() = default;

        void CreateGeometry();
        void Draw();

        uint32_t vao, vbo; 
        std::vector<float> vertices; // 10 vertical and 10 horizontal lines, each requiring 4 vertices
         
        Ref<Material>  material;

    };



    void Navigation::CreateGeometry()
    {
        // Vertices for the axes: (x, y, z, r, g, b)
        vertices = {
            
            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // X-axis (Red)
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // Y-axis (Green)
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // Z-axis (Blue)
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };

        // Generate and bind VAO and VBO
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Unbind VBO and VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    void Navigation::Draw()
    { 
        glDisable(GL_DEPTH_TEST);

        material->Bind(); 
        glBindVertexArray(vao); 

        glm::mat4 model = glm::mat4(1.0f); 
        material->GetShader()->SetMat4("u_Model", model);



       //glm::mat4 projection_view = Renderer::GetMainCamera()->GetProjectionViewMatrix();
       //material->GetShader()->SetMat4("u_ProjectionView", projection_view);

        glm::mat4  view = Renderer::GetMainCamera()->GetViewMatrix();
        //view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        glm::mat4  projection = Renderer::GetMainCamera()->GetProjectionMatrix();
        
        material->GetShader()->SetMat4("u_ProjectionView", projection * view);

        glViewport(0,0, 2560/4, 1440/4);
        glDrawArrays(GL_LINES, 0, 6);
         
        glBindVertexArray(0);
        material->Unbind(); 

        glEnable(GL_DEPTH_TEST);
    }

     

}

 