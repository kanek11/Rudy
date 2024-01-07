#pragma once

#include "RudyPCH.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Object.h"
  
#include "Rudy/Renderer/Renderer.h"

namespace Rudy {

    //<<terminal>>
    class Cube : public MeshObject {
    public:
        ~Cube() = default;

        Cube() : MeshObject()
        { 
            this->GetRendererComponent()->SetMesh( CreateMeshGeometry() );
        }
      

        Ref<Mesh> CreateMeshGeometry(); 
      
      

        void DrawSkybox(Ref<Camera> cam);  //need to set special view matrix, and gldepthfunc;
  
        static Ref<Cube> Create() { return std::make_shared<Cube>(); }
        

    }; 

}