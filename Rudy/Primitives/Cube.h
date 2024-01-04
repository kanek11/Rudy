#pragma once

#include "RudyPCH.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Object.h"
  
#include "Rudy/Renderer/Renderer.h"

namespace Rudy {


    class Cube : public MeshObject {
    public:
        Cube() : MeshObject()
        { 
            SetMesh( CreateMeshGeometry() );   
        }
        ~Cube() = default;

        Ref<Mesh> CreateMeshGeometry();

      
        void DrawSkybox();  //need to set special view matrix, and gldepthfunc;
  
        static Ref<Cube> Create() { return std::make_shared<Cube>(); }
        

    }; 

}