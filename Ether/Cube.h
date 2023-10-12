#pragma once

#include "EtherPCH.h"
#include "Hazel/Renderer/Mesh.h"
#include "Hazel/Renderer/Material.h"
#include "Hazel/Renderer/Object.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Hazel/Renderer/Renderer.h"

namespace Hazel {


    class Cube:public MeshObject {
    public:
        Cube() { CreateGeometry(); }
        ~Cube() = default;

        void CreateGeometry();

        void Draw() override;
        void DrawSkybox();
 

        unsigned int CubeVAO, CubeVBO, CubeEBO;
        //Ref<Shader>  Shader;   

    };


    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    


}