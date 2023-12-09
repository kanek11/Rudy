#pragma once

#include "RudyPCH.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Object.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Rudy/Renderer/Renderer.h"

//a plane , lying on the xz plane , facing up;   i.e.  -1,0,1 -1,0,-1  1,0,-1  1,0,1
//basically a reference plane for the world space;  
//if attached material , it will be rendered;  otherwise will in wireframe mode;
 

//for now, i want the pre-defined model to be self-contained. not relying on object class design.

//all public, free to set;

namespace Rudy {


    class Plane : public MeshObject {
    public: 
        ~Plane() = default;

        Plane(float size) { 
            CreateGeometry(size); 
            mesh->SetupVertices();
            mesh->LoadToGPU();
        }

        void CreateGeometry(float size);

        static Ref<Plane> Create(float size) { return CreateRef<Plane>(size); }


        //void Draw() override;  

       // Ref<Mesh>  Mesh;
       // Ref<Material>  Material;
         
    };
      
    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    void Plane::CreateGeometry(float size)
    {

        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices = {0,1,2, 0,2,3};

        
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

        //generate vertices 
        Vertex leftBottom{pos1 * size ,normal,tangent,uv1};
        Vertices.push_back(leftBottom);
         
        Vertex leftTop{ pos2 * size ,normal,tangent,uv2};
        Vertices.push_back(leftTop);

        Vertex rightTop{ pos3 * size,normal, tangent, uv3};
        Vertices.push_back(rightTop);
        
        Vertex rightBottom{ pos4 * size,normal, tangent, uv4};
        Vertices.push_back(rightBottom); 

        //generate mesh
        auto _mesh = Mesh::Create();  
        _mesh->vertices = Vertices;
        _mesh->indices = Indices;

        mesh = _mesh;

    }
    
    
    
    
    
   // void Plane::Draw()
   // {
   //     //glBindVertexArray(mesh->GetVertexArray());
   //     //glBindVertexArray(0); 
   //
   //     mesh->Bind(); 
   //     material->Bind();
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
   //     material->GetShader()->SetMat4("u_Model", model);
   //     material->GetShader()->SetMat4("u_View", view);
   //     material->GetShader()->SetMat4("u_Projection", projection);
   //
   //     //wireframe mode  
   //
   //    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   //     glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
   //
   //     // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  
   //
   //     material->Unbind();
   //     mesh->Unbind(); 
   //
   // }
     

}





 