#pragma once

#include "RudyPCH.h"

#include "Rudy/Renderer/Renderer.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Object.h"



namespace Rudy {


    class ScreenQuad : public MeshObject {
    public:
        ~ScreenQuad() = default;

        ScreenQuad() : MeshObject() 
        { 
          this->SetMesh( CreateMeshGeometry() );  
        }

        Ref<Mesh> CreateMeshGeometry(); 

        static Ref<ScreenQuad> Create() { return std::make_shared<ScreenQuad>(); }
         
    };
     

    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    Ref<Mesh> ScreenQuad::CreateMeshGeometry()
    {

        auto _mesh = Mesh::Create();
        _mesh->topology = MeshTopology::TRIANGLES;
        _mesh->drawCommand = MeshDrawCommand::INDEXED;
        _mesh->indices = { 0,1,2, 1,3,2 };

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
         
        return _mesh;

 
    }

 



}