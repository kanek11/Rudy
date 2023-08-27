#pragma once

#include "EtherPCH.h"

#include <Hazel/Renderer/Mesh.h>
#include <Hazel/Renderer/Material.h>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
 

namespace Hazel {

    struct Transform
    {
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);

    };

    class Object
    {
    public :
        Object()
        {
            m_Name = "Default Object";
            m_Shader = Shader::Create("blinn-phong", "Resources/Shaders/BlinnPhong_VS.glsl", "Resources/Shaders/BlinnPhong_FS.glsl");
        }

    //private:
     public:
        std::string m_Name; 
        std::string m_Directory;

        Transform m_Transform;

        std::vector<Ref<Mesh>> m_Meshes;
        std::vector<Ref<Material>> m_Materials;
        Ref<Shader> m_Shader;
         

    public:
        std::vector<Ref<Texture2D>> m_Loaded_Textures;  //opt to avoid duplicate textures,  loading is extremely slow

        void loadModel(std::string const& path);
        void processNode(aiNode* node, const aiScene* scene);
        Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
        Ref<Material> processMaterial(aiMesh* mesh, const aiScene* scene);


        //void DrawModel();

    };
     
}
  

 