#pragma once

#include "RudyPCH.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Rudy/Renderer/Renderer.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Object.h"



namespace Rudy {


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