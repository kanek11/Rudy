#pragma once

#include "EtherPCH.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/Object.h"



namespace Hazel {


    class Quad : Object {
    public:
        Quad() { CreateGeometry(); }
        ~Quad() = default;


        void Draw() override;

        void SetMaterial(Ref<Material> material) { m_Material = material; }
        Ref<Material> GetMaterial() { return m_Material; }

        void CreateGeometry(); 

        uint32_t QuadVAO, QuadVBO, QuadEBO;
        std::vector<unsigned int> Indices{ 0,1,2, 1,2,3 };


        Ref<Material> m_Material;
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

        m_Material->Bind();

        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, nullptr);

        m_Material-> Unbind();

        glBindVertexArray(0);  
    }




}