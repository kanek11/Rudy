#pragma once

#include "RudyPCH.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Object.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Rudy/Renderer/Renderer.h"

namespace Rudy {


    class Cube : public MeshObject {
    public:
        Cube() { CreateGeometry();  
        mesh->SetupVertices();
        mesh->LoadToGPU(); 
        }
        ~Cube() = default;

        void CreateGeometry();

        void Draw(Ref<Camera> cam) override;
        void DrawSkybox();  //need to set special view matrix, and gldepthfunc;
  
        //uint32_t CubeVAO, CubeVBO, CubeEBO;
        //Ref<Shader>  Shader;   

    };


    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    


}