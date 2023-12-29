#pragma once
  
#include "RudyPCH.h"  
 
#include <glm/glm.hpp> 
 

//TODO: rewrite the visibility once the API is stab
// 


//mesh mainly handles the geometry data; 
//the drawcall is handled by the renderer; 
// the processing is handled by the engine; to be designed;
//https://docs.unity3d.com/2023.3/Documentation/ScriptReference/Mesh.html



//some workflow might prefer each fields to be set individually; instead of a vertex struct;
//and we should standardize the creation in this way; 

//comes the problem, "when" to setup the mesh for the GPU;   
//should be handled by the pipeline, not the user; 
//it's trivial to ask the user to say "it's done";   kinda poor design;
// 
//as resouce management is generally a problem for the engine;  say, we might need to load-unload the data;


//for now, let's use a basic plan;
//should be done once, once pass to GPU,  modifications are done by shaders; 

//or we know that it must be "done"  before the [first] drawcall; not every frame;



//#define MAX_BONE_INFLUENCE 4 


namespace Rudy {


    //the "topo" determines the interpretation of the index buffer;  and hence the drawcall;
    enum class MeshTopology {
		TRIANGLES,   //triangle is THE default for games; 
        POINTS, 
        LINES, 
        STRIP,
        QUADS,
	};


    //standard 4 bones per vertex
    //designed to be a effective array ;
    //for now it can avoid more nested struct , and low-level vector representation;

    //make sure the fields
    struct BoneWeight
    {
        int BoneIndex0;
        int BoneIndex1;
        int BoneIndex2; 
        int BoneIndex3;
         
        float Weight0; 
        float Weight1; 
        float Weight2; 
        float Weight3;
        
	};

     

    //pre-defined vertex DS , to be used by model loader/creator
    //the loader neednot to care the API setup but to fill the data.
    struct Vertex {
        glm::vec3 Position= glm::vec3(0,0,0);
        glm::vec3 Normal= glm::vec3(0,0,0);
        glm::vec3 Tangent= glm::vec3(0,0,0);
        glm::vec2 UV =  glm::vec2(0,0);

        //
        glm::ivec4 BoneIndices = glm::ivec4(-1);  //invalid
        glm::vec4 BoneWeights = glm::vec4(0,0,0,0);

        Vertex () = default;
        Vertex(glm::vec3 position, glm::vec3 normal, glm::vec3 tangent, glm::vec2 uv, 
            glm::ivec4 boneIndices = glm::ivec4(0,0,0,0), glm::vec4 boneWeights = glm::vec4(0,0,0,0))
            : Position(position), Normal(normal), Tangent(tangent), UV(uv), BoneIndices(boneIndices), BoneWeights(boneWeights) {}
    };

 


    class Mesh {
    public: 
        ~Mesh() = default;
        Mesh() = default;
 

        virtual void SetupVertices() = 0;  //to data-oriented structure;
        virtual void LoadToGPU() = 0;
        
        virtual void Bind() = 0;
        virtual void Unbind() = 0; 

        //for drawcall.
        virtual uint32_t GetVertexArray() = 0;
        virtual uint32_t GetIndexCount() = 0;
          
        //interface creation;
        static Ref<Mesh> Create(); 

    public: 
        //interface fields for easy to define;
        //some workflow might prefer each fields to be set individually; instead of a vertex struct;
        //and we should standardize in this way;  not expose the vertex struct to the user;

        std::vector<glm::vec3>  positions;
        std::vector<glm::vec3>  normals; 
        std::vector<glm::vec3>  tangents;
        std::vector<glm::vec2>  UVs; 

        std::vector<BoneWeight>  boneWeights; 

        std::vector<uint32_t> indices;
        MeshTopology topology = MeshTopology::TRIANGLES;


    public: 
        //lowe-level GPU data;

        uint32_t m_vertexBufferTarget, m_indexBufferTarget, m_vertexArrayTarget;
        std::vector<Vertex>  vertices;  
        
        //std::vector<uint32_t>  m_indices;  

    };

}

 