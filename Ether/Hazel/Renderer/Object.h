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


    //object is the base class for all objects in the scene,
    //design carefully:   cannot be easily changed.
    class Object
    {
    public :
        ~Object() = default;
        Object() = default;

    //private:
     public:
        std::string Name; 
        Transform Transform;


    };
 
 

     class MeshObject : public Object
     {
     public:

         ~MeshObject() = default;
         MeshObject() = default;
         MeshObject(Ref<Mesh> mesh, Ref<Material> material);
     
     
         static Ref<MeshObject> Create(Ref<Mesh> mesh, Ref<Material> material);
         //static Ref<MeshObject> CreateFromFile(std::string const& path);
          
     
         Ref<Mesh> m_Mesh;
         Ref<Material> m_Material;
         //Ref<Shader> m_Shader;
      
     
     };



     class Hierarchy : public Object
     {
     public:

         ~Hierarchy() = default;
         Hierarchy() = default;

         Hierarchy(std::string const& path);
     
     
         static Ref<Hierarchy> CreateFromFile(std::string const& path);
     
         std::vector< Ref<MeshObject> >  m_MeshObjects;
         std::string m_Directory;

     private: 
         std::vector<Ref<Texture2D>> m_Loaded_Textures;  
         //opt to avoid duplicate textures,  loading is extremely slow


         void processNode(aiNode* node, const aiScene* scene);
         Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
         Ref<Material> processMaterial(aiMesh* mesh, const aiScene* scene);
     };
     
 


     
}
  

 