#pragma once

#include "RudyPCH.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
 

#include <Rudy/Renderer/Mesh.h>
#include <Rudy/Renderer/Material.h> 

#include <Rudy/Renderer/Camera.h> 
#include <Rudy/Renderer/Transform.h> 
#include <Rudy/Renderer/Buffer.h>


#include <Rudy/Animation/Animation.h> 

#include <Rudy/Renderer/RendererComponent.h>

namespace Rudy {



    //object is the base class for all objects in the scene,  
    //we will have reflection system in the future,  but for now,  we just use this simple inheritance;

    //virtual destructor is needed to correctly release derived class;
    //<<abstract>>
    class Object
    {
    public :
        virtual ~Object() = default;
        Object() 
        { 
			transform = CreateRef<Transform>(); 
		}
      
  
     public:
         Ref<Transform> transform = nullptr;
         std::string name = "Unnamed Object";

         bool isRenderable = false;

    };

    
     
    //a renderable object is an object that can be rendered;
    //now, it contains a renderer component; 
    //a generic renderer component defines draw command. 
    //there's no default one,  user must define the drived classes and set the renderer component;
 

    //we hardcode the transform component in the renderer component;  
    //todo: maybe implement a fancy ECS system in the future;
    //<<abstract base>>
    class RenderableObject : public Object
    {
    public: 
        virtual ~RenderableObject() = default; 
       RenderableObject() : Object() { isRenderable = true; } 
     

        void SetRendererComponent(Ref<RendererComponent> rendererComponent) 
		{ 
            rendererComponent->SetTransform(this->transform);
			this->rendererComponent = rendererComponent; 
		}

        Ref<RendererComponent> GetRendererComponent() 
        { return rendererComponent; }
         
  
        bool hasRendererComponent() { return rendererComponent != nullptr; }
    
    public:

        virtual void Draw(Ref<Camera> cam) = 0;

        Ref<RendererComponent> rendererComponent = nullptr;
    };
 




    //<<terminal>>
    //simply assume mesh renderer component; come with a mesh component;
    class MeshObject : public Object
    {
    public:
        virtual ~MeshObject()  = default;
        MeshObject() : Object() {
            isRenderable = true;
            rendererComponent = MeshRendererComponent::Create();
            rendererComponent->SetTransform(this->transform);
        }   

        static Ref<MeshObject> Create() 
		{ return CreateRef<MeshObject>(); }


        void SetRendererComponent(Ref<MeshRendererComponent> rendererComponent)
        { 
            this->rendererComponent = rendererComponent;
        }

        Ref<MeshRendererComponent> GetRendererComponent()
        {
            return this->rendererComponent;
        }  

        void SetMaterial(Ref<Material> mat)
		{
			this->rendererComponent->SetMaterial(mat);
		} 
        void SetShader(Ref<Shader> shader)
        { 
            this->rendererComponent->SetShader(shader);
		}


    public:
        //override draw command if needed;
        virtual void Draw(Ref<Camera> cam)
        {
            this->rendererComponent->Draw(cam);
        }
         
        Ref<MeshRendererComponent> rendererComponent = nullptr;

    };


      

     //a model contains... whatever the model contains;  
     //expect multiple mesh objects;
     class Model    
     {
     public:
         //global setting ; statics

         static float s_scaleFactor;  
 
     public:  
         ~Model() = default; 
         Model() = default;
         Model(std::string const& path); 
     
         static Ref<Model> LoadModel(std::string const& path);

 
     public:
         std::vector< Ref<MeshObject> >  meshObjects;


         std::string directory;


         //animation and bound bones are could-be-separate;
         //might need retarget;
         Ref<AnimationClip> animationClip;
          
         //retreive the bound bones defined in the mesh;     //aiBone->mName; aiBone->mOffsetMatrix 
         //ordered map,  key = name to facilate searching by name in animation keyframe; 
         std::map<std::string, Bone> bindPoseBoneMap;
          

	 public:  
         void Draw(Ref<Camera> cam); 
          
         void SetShader(Ref<Shader> shader) ;


 //========system 
         //retrieved in the hierarchy of the animation file;
         Ref<Object> rootNode = CreateRef<Object>();
         Ref<Object> findNode(std::string const& name);
         Ref<Object> findNodeRecursive(Ref<Transform> node, std::string const& name);
          

     public:  
         std::vector<Ref<Texture2D>> m_Loaded_Textures;  
         //opt to avoid duplicate textures,  loading is extremely slow


         void processNode(const aiScene* scene, aiNode* ai_node, Ref<Object> scene_node);

         Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
         Ref<Material> processMaterial(aiMesh* mesh, const aiScene* scene);
          
         void processBones(const aiScene* scene, aiMesh* mesh, std::vector<Vertex>& vertices);
         void processAnimation(const aiScene* scene);


     };
     
 

       


     
}
  

 