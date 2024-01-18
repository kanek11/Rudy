#pragma once

#include "RudyPCH.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Object.h"
 

#include "Rudy/Renderer/Renderer.h"

//a plane , lying on the xz plane , facing up;   i.e.  -1,0,1 -1,0,-1  1,0,-1  1,0,1
//basically a reference plane for the world space;  
//if attached material , it will be rendered;  otherwise will in wireframe mode;
 

//for now, i want the pre-defined model to be self-contained. not relying on object class design.

//all public, free to set;

namespace Rudy {


    class Plane : public MeshObject {
    public: 
        ~Plane() = default;

        Plane(uint32_t subdivison);
        static Ref<Plane> Create(uint32_t subdivison);

        Ref<Mesh> CreateMeshGeometry(uint32_t subdivison); 
         
    };
      
  
    
    
}





 