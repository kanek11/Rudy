#pragma once
 

#include "Ether.h"
#include "Hazel/Core/Base.h"

#include <vector>
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

    //struct Texture {
    //    unsigned int id;
    //    string type;
    //    string path;
    //};



    class Mesh {
    public: 
        //Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);  //vector<Texture> textures);
        ~Mesh() = default;
 
        static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

        virtual void SetupMesh() = 0;
        virtual void DrawMesh() =0 ;

        virtual unsigned int GetVertexAarry() = 0;

    private:
        unsigned int m_VBO, m_EBO, m_VAO;
        std::vector<Vertex>       m_vertices;
        std::vector<unsigned int>  m_indices;
        //std::vector<Texture>      textures; 

    };

}

