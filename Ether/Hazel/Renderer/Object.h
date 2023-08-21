#pragma once

#include "EtherPCH.h"

#include <Hazel/Renderer/Mesh.h>
#include <Hazel/Renderer/Material.h>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;


namespace Hazel {

    struct Transform
    {
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);

    };

    class Object
    {
    private:
        std::string m_Name;

        Transform m_Transform;

        std::vector<Ref<Mesh>> m_Meshes;
        std::vector<Ref<Material>> m_Materials;

        std::string m_Directory;





    };

}
  

 