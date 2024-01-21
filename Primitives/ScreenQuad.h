#pragma once

#include "RudyPCH.h"

#include "Rudy/Renderer/Renderer.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Object.h"



namespace Rudy {


    class ScreenQuad : public MeshObject {
    public:
        ~ScreenQuad() = default; 
        ScreenQuad();

        static Ref<ScreenQuad> Create();

        Ref<Mesh> CreateMeshGeometry(); 


    };
     




}