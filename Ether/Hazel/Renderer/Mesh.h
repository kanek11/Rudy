#pragma once
 

#include "EtherPCH.h"  
 
#include <glm/glm.hpp>


 

//TODO: rewrite the visibility once the API is stable.
 

//#define MAX_BONE_INFLUENCE 4



namespace Hazel {

    //pre-defined vertex DS , to be used by model loader/creator
    //the loader neednot to care the API setup but to fill the data.
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
        //int m_BoneIDs[MAX_BONE_INFLUENCE];
        //float m_Weights[MAX_BONE_INFLUENCE];
    };

 


    class Mesh {
    public: 
        ~Mesh() = default;
 

        virtual void SetupMesh() = 0;
        
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        //virtual void DrawMesh() =0 ;

        virtual unsigned int GetVertexAarry() = 0;
        virtual unsigned int GetIndexCount() = 0;


        static Ref<Mesh> Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    private:
        unsigned int m_VBO, m_EBO, m_VAO;
        std::vector<Vertex>       m_vertices;  
        std::vector<unsigned int>  m_indices;  //or Faces in Matlab;
        //std::vector<Texture>      textures; 

    };

}


//cpp


