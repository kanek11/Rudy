#pragma once

#include "EtherPCH.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/Object.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Hazel/Renderer/Renderer.h"

namespace Hazel {


    class Cube {
    public:
        Cube() { CreateGeometry(); }
        ~Cube() = default;

        void CreateGeometry();
        void DrawCube();
        void DrawSkybox();

        Transform  Transform;
        //Ref<Mesh>  Mesh;
        Ref<Material>  Material;

        unsigned int CubeVAO, CubeVBO, CubeEBO;
        //Ref<Shader>  Shader;   

    };


    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    void Cube::CreateGeometry()
    {
        //we don't use indices for now
        float CubeVertices[] = {        
              -1.0f,  1.0f, -1.0f,
              -1.0f, -1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,
               1.0f,  1.0f, -1.0f,
              -1.0f,  1.0f, -1.0f,
              
              -1.0f, -1.0f,  1.0f,
              -1.0f, -1.0f, -1.0f,
              -1.0f,  1.0f, -1.0f,
              -1.0f,  1.0f, -1.0f,
              -1.0f,  1.0f,  1.0f,
              -1.0f, -1.0f,  1.0f,
              
               1.0f, -1.0f, -1.0f,
               1.0f, -1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,
              
              -1.0f, -1.0f,  1.0f,
              -1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f, -1.0f,  1.0f,
              -1.0f, -1.0f,  1.0f,
              
              -1.0f,  1.0f, -1.0f,
               1.0f,  1.0f, -1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
              -1.0f,  1.0f,  1.0f,
              -1.0f,  1.0f, -1.0f,
              
              -1.0f, -1.0f, -1.0f,
              -1.0f, -1.0f,  1.0f,
               1.0f, -1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,
              -1.0f, -1.0f,  1.0f,
               1.0f, -1.0f,  1.0f
        };

    
        
        glGenVertexArrays(1, &CubeVAO);
        glGenBuffers(1, &CubeVBO);
        glBindVertexArray(CubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), &CubeVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


        HZ_CORE_WARN("cube: CubeVAO: {0} is created", CubeVAO);


    }

     
    void Cube::DrawSkybox()
    { 
        if(Material == nullptr)
            HZ_CORE_WARN("Cube::DrawSkybox() Material is nullptr");
        else
            Material->Bind();

        auto skyboxShader = Material->GetShader();
        skyboxShader->Bind(); 

        //auto view = glm::mat4(glm::mat3(Renderer::GetMainCamera()->GetViewMatrix())) ;  //error: undefined behaviour.
        auto view = Renderer::GetMainCamera()->GetViewMatrix();
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        auto projection = Renderer::GetMainCamera()->GetProjectionMatrix();
        skyboxShader->SetMat4("u_View", view ); // remove translation from the view matrix
        skyboxShader->SetMat4("u_Projection", projection);


        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

        // skybox cube
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36); 
        glBindVertexArray(0);
         
        glDepthFunc(GL_LESS); // set depth function back to default


        if (Material == nullptr)
            HZ_CORE_WARN("Cube::DrawSkybox() Material is nullptr");
        else
            Material->Unbind();
        
    }




}