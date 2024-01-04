#pragma once

#include "RudyPCH.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
 

#include <Rudy/Renderer/Mesh.h>
#include <Rudy/Renderer/Material.h> 
#include <Rudy/Renderer/Camera.h>

#include <Rudy/Renderer/Animation.h>

#include <Rudy/Renderer/Transform.h>

#include <Rudy/Renderer/Buffer.h>


namespace Rudy {



    //object is the base class for all objects in the scene,  
    //we will have ECS and reflection system later,  but for now,  we just use this simple inheritance;

    class Object
    {
    public :
        ~Object() = default;
        Object() = default; 
      
        //assume renderable obj;  offer drawing function;
        //as a container, object is not abstract; we donot impose virtual draw here;


     public:
        Ref<Transform> transform  = CreateRef<Transform>();
        std::string name;  

    };

    
    //Å¢§Œ design ;  
    class RenderableObject : public Object
    {
    public:
        virtual void Draw(Ref<Camera> cam) = 0;
    };
 

    //<MeshRederer> equivalent;
    //takes a mesh and material;  
    //handles buffers , draw calls;

     class MeshObject : public RenderableObject
     {
    //========user:

     public:
         static Ref<MeshObject> Create(); 

        //GPU:
         void SetupMeshBuffers();


     public:
   

         void SetMesh(Ref<Mesh> m) 
         { 
             mesh = m; 
             //prepare
             mesh->SetupVertices();
             SetupMeshBuffers();
         }
         Ref<Mesh> GetMesh() { return mesh; }

         void SetVertexArray(Ref<VertexArray> vao) { vertexArray = vao; }
         Ref<VertexArray> GetVertexArray() { return vertexArray; }

         void SetMaterial(Ref<Material> mat ) { material = mat ; }
         Ref<Material> GetMaterial() { return material; }

         bool hasMaterial() { return material != nullptr; }
         bool hasMesh() { return mesh != nullptr; }


      //=======system; 
         ~MeshObject() = default;
         MeshObject()
         {
			 vertexArray = VertexArray::Create(); 
             vertexBuffer = VertexBuffer::Create();
             indexBuffer = IndexBuffer::Create(); 

             RD_CORE_INFO("MeshObject ccreated");
         }


         void Draw(Ref<Camera> cam) override;
         void DrawInstanced(Ref<Camera> cam, uint32_t count);


         //new: vao is not limited to mesh(geometry) anymore;
         Ref<VertexArray> vertexArray;
         Ref<VertexBuffer> vertexBuffer;
         Ref<IndexBuffer> indexBuffer;

         Ref<Mesh> mesh;
         Ref<Material> material;

     };



      

     //a model contains... whatever the model contains;  
     //expect multiple mesh objects;
     class Model  : public RenderableObject
     {
     //========user:
     public:

         static float ScaleFactor;  //todo:  make it a parameter;
     
         static Ref<Model> LoadModel(std::string const& path);

 
        public:
         std::vector< Ref<MeshObject> >  meshObjects;
         std::string directory;


         //animation and bound bones are could-be-separate;
         //might need retarget;
         Ref<AnimationClip> animationClip;

         //retrieved in the hierarchy of the animation file;
         Ref<Object> rootNode = CreateRef<Object>();
         Ref<Object> findNode(std::string const& name);
         Ref<Object> findNodeRecursive(Ref<Transform> node, std::string const& name);

         
         //retreive the bound bones in the mesh; 
         //ordered map,  key = name to facilate searching by name; in animation keyframe;
         //aiBone->mName;  //aiBone->mOffsetMatrix 
         std::map<std::string, Bone> bindPoseBoneMap;



     //========system 

	 public: 

         ~Model() = default;
         Model() = default;

         Model(std::string const& path);


         void Draw(Ref<Camera> cam) override;


         void SetMaterial(Ref<Material> mat) ;
         void SetShader(Ref<Shader> shader) ;


     private: 
         std::vector<Ref<Texture2D>> m_Loaded_Textures;  
         //opt to avoid duplicate textures,  loading is extremely slow


         void  processNode(const aiScene* scene, aiNode* ai_node, Ref<Object> scene_node);

         Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
         Ref<Material> processMaterial(aiMesh* mesh, const aiScene* scene);
          
         void processBones(const aiScene* scene, aiMesh* mesh, std::vector<Vertex>& vertices);
         void processAnimation(const aiScene* scene);


     };
     
 

       


     
}
  

 