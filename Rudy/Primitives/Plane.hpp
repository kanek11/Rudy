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

        Plane(float size): MeshObject()
        { 
            this->GetRendererComponent()->SetMesh( CreateMeshGeometry(size) );
        }

        Ref<Mesh> CreateMeshGeometry(float size);

        static Ref<Plane> Create(float size) { return CreateRef<Plane>(size); }
 
         
    };
      
    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    Ref<Mesh> Plane::CreateMeshGeometry(float size)
    {

        std::vector<glm::vec3> _positions = {
			glm::vec3(-1.0, 0.0, 1.0) * size,  //left bottom 
			glm::vec3(-1.0, 0.0,-1.0) * size,  //left top
			glm::vec3( 1.0, 0.0,-1.0) * size,  //right top
			glm::vec3( 1.0, 0.0, 1.0) * size,  //right bottom
		};

        std::vector<uint32_t> _indices = {0,1,2, 0,2,3};

     
        // texture coordinates,  recall opengl is bottom left origin
        std::vector<glm::vec2> _uvs = {
			glm::vec2(0.0, 0.0),
			glm::vec2(0.0, 1.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(1.0, 0.0)
		};
      

        //defined in local tangent space ;  TBN is right handed, xyz respectivelyf
        //they are the same for all vertices
        glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);  //y axis  world up
        glm::vec3 tangent = glm::vec3(1.0, 0.0, 0.0);  //x axis 
 

        //generate mesh
        auto _mesh = Mesh::Create();  
        _mesh->positions = _positions;
        _mesh->UVs = _uvs;
        _mesh->normals = std::vector<glm::vec3>(_positions.size(), normal);
		_mesh->tangents = std::vector<glm::vec3>(_positions.size(), tangent);

        _mesh->indices = _indices;

        _mesh->topology = MeshTopology::TRIANGLES;
        _mesh->drawCommand = MeshDrawCommand::INDEXED;


        return _mesh;

    }
    
    
    
}





 