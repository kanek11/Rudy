#pragma once

#include "EtherPCH.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/Object.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Hazel/Renderer/Renderer.h"

//a plane , lying on the xz plane , facing up;   i.e.  -1,0,1 -1,0,-1  1,0,-1  1,0,1
//basically a reference plane for the world space;  
//if attached material , it will be rendered;  otherwise will in wireframe mode;
 

//for now, i want the pre-defined model to be self-contained. not relying on object class design.

//all public, free to set;

namespace Hazel {


    class Plane : public MeshObject {
    public:
        Plane(float size) { CreateGeometry(size); }
        ~Plane() = default;

        void CreateGeometry(float size);
        //void Draw() override;  

       // Ref<Mesh>  Mesh;
       // Ref<Material>  Material;
         
    };
      
    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    void Plane::CreateGeometry(float size)
    {

        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices = {0,1,2, 0,2,3};

        
        glm::vec3 pos1(-1.0, 0.0, 1.0);  //left bottom 
        glm::vec3 pos2(-1.0, 0.0,-1.0);  //left top
        glm::vec3 pos3( 1.0, 0.0,-1.0);  //right top
        glm::vec3 pos4( 1.0, 0.0, 1.0);  //right bottom
        // texture coordinates,  recall opengl is bottom left origin
        glm::vec2 uv1(0.0, 0.0);
        glm::vec2 uv2(0.0, 1.0);
        glm::vec2 uv3(1.0, 1.0);
        glm::vec2 uv4(1.0, 0.0);


        //defined in local tangent space ;  TBN is right handed, xyz respectivelyf
        glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);  //y axis  world up
        glm::vec3 tangent = glm::vec3(1.0, 0.0, 0.0);  //x axis
        glm::vec3 bitangent = glm::vec3(0.0, 0.0, 1.0); //z axis
 

        //generate vertices 
        Vertex leftBottom{pos1 * size ,normal,uv1,tangent,bitangent};
        Vertices.push_back(leftBottom);
         
        Vertex leftTop{ pos2 * size ,normal,uv2,tangent,bitangent };
        Vertices.push_back(leftTop);

        Vertex rightTop{ pos3 * size,normal,uv3, tangent,bitangent };
        Vertices.push_back(rightTop);
        
        Vertex rightBottom{ pos4 * size,normal,uv4, tangent,bitangent };
        Vertices.push_back(rightBottom); 

        //generate mesh
        m_Mesh = Mesh::Create(Vertices, Indices);  

    }
    
    
    
    
    
   // void Plane::Draw()
   // {
   //     //glBindVertexArray(m_Mesh->GetVertexArray());
   //     //glBindVertexArray(0); 
   //
   //     m_Mesh->Bind(); 
   //     m_Material->Bind();
   //
   //     // set uniforms for transforms
   //     // identity matrix for now;
   //
   //     glm::mat4 model = glm::mat4(1.0f);
   //     model = glm::scale(model, Transform.Scale);
   //     model = glm::translate(model, Transform.Position);
   //
   //
   //     glm::mat4 view = Renderer::GetMainCamera()->GetViewMatrix(); 
   //     glm::mat4 projection = Renderer::GetMainCamera()->GetProjectionMatrix();
   //
   //     m_Material->GetShader()->SetMat4("u_Model", model);
   //     m_Material->GetShader()->SetMat4("u_View", view);
   //     m_Material->GetShader()->SetMat4("u_Projection", projection);
   //
   //     //wireframe mode  
   //
   //    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   //     glDrawElements(GL_TRIANGLES, m_Mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
   //
   //     // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  
   //
   //     m_Material->Unbind();
   //     m_Mesh->Unbind(); 
   //
   // }

    


    class WorldGrid { 
     
    public:
		WorldGrid(int size) { CreateGeometry(size); }
		~WorldGrid() = default;

		void CreateGeometry(int size);
		void Draw();


        GLuint gridVAO, gridVBO;

        //address geometry differently
        std::vector<float> gridVertices; // 10 vertical and 10 horizontal lines, each requiring 4 vertices
 
		 Transform  Transform;
		//Ref<Mesh>  Mesh;
		 Ref<Material>  Material;
		 //Ref<Shader>  Shader;    
	};



    //only geometry.  no material, no shader;
    //modern opengl doesn't have draw grid kind of function.  the grid is done by drawing lines.  

    //use two vertices to draw a line.  
    //the grid is centered at origin;  the size is the number of grids in each direction;  
    //i.e.  size = 10,  then the grid is 21x21;  vary from -10 to 10;   
    // 
    //the total number of vertices must be even.  vertices must be in pairs.


    void WorldGrid::CreateGeometry(int size)
    {
        HZ_CORE_INFO("WorldGrid::CreateGeometry");
          
   
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
        glBufferData(GL_ARRAY_BUFFER, gridVertices.size()*sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        HZ_CORE_INFO("worldgrid: data passed, size:{0}", gridVertices.size() );

        // Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0); 

    }
    
     

    void WorldGrid::Draw()
    {
      

        Material->Bind();
        glBindVertexArray(gridVAO);


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, Transform.Scale);
        model = glm::translate(model, Transform.Position);
        glm::mat4 view = Renderer::GetMainCamera()->GetViewMatrix();

        glm::mat4 projection = Renderer::GetMainCamera()->GetProjectionMatrix();

        Material->GetShader()->SetMat4("u_Model", model);
        Material->GetShader()->SetMat4("u_View", view);
        Material->GetShader()->SetMat4("u_Projection", projection);

        glDrawArrays(GL_LINES, 0, gridVertices.size()/3);  // 3 because each vertex has 3 floats.
     
        glBindVertexArray(0);

        Material->Unbind();

    }



 







}





 