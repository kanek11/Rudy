#pragma once

#include "EtherPCH.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/Object.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Hazel/Renderer/Renderer.h"

namespace Hazel {


    class Quad {
    public:
        Quad() { CreateGeometry(); }
        ~Quad() = default;

        void CreateGeometry();
        void Draw();

        uint32_t QuadVAO, QuadVBO, QuadEBO;
        Ref<Shader> Shader; 
        std::vector<unsigned int> Indices{ 0,1,2, 1,2,3 };
    };
     

    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    void Quad::CreateGeometry()
    {

        float quadVertices[] = {
            // positions        // texture Coords  
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };
 
       //float quadVertices[] = {
       //    // positions        // texture Coords  //debug:: leave a margin for purpose
       //    -1.0f,  1.0f, 0.0f,  -0.3f, 1.3f, 
       //    -1.0f, -1.0f, 0.0f,  -0.3f, -0.3f,
       //     1.0f,  1.0f, 0.0f,  1.3f,  1.3f,
       //     1.0f, -1.0f, 0.0f,  1.3f,  -0.3f,
       //};

        // setup plane VAO
        glGenVertexArrays(1, &QuadVAO);
        glGenBuffers(1, &QuadVBO);
        glGenBuffers(1, &QuadEBO);

        glBindVertexArray(QuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);
         
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    }




    void Quad::Draw()
    {
        glBindVertexArray(QuadVAO);
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);  
    }




}