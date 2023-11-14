#pragma once

#include "EtherPCH.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/Object.h"



namespace Hazel {


    class Quad : public MeshObject {
    public:
        Quad() { CreateGeometry();
        mesh->SetupVertices();
        mesh->LoadToGPU();
        }
        ~Quad() = default; 

        void CreateGeometry(); 
         
    };
     

    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    void Quad::CreateGeometry()
    {

        auto _mesh = Mesh::Create();
        _mesh->indices = { 0,1,2, 1,2,3 };

        //float quadVertices[] = {
        //    // positions        // texture Coords  
        //    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        //    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        //     1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        //     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        //};

        _mesh->positions = {
         glm::vec3(-1.0f,  1.0f, 0.0f),
         glm::vec3(-1.0f, -1.0f, 0.0f),
         glm::vec3(1.0f,  1.0f, 0.0f),
         glm::vec3(1.0f, -1.0f, 0.0f)
        };

        _mesh->UVs = {

         glm::vec2(0.0f, 1.0f),
         glm::vec2(0.0f, 0.0f),
         glm::vec2(1.0f, 1.0f),
         glm::vec2(1.0f, 0.0f)

		}; 

        mesh = _mesh;

 
    }

 



}