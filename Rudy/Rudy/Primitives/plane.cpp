#include "RudyPCH.h"

#include "Plane.h"


namespace Rudy
{

    Plane::Plane(uint32_t subdivison) : StaticMeshObject()
    {
        this->GetRenderer()->SetMesh(CreateMeshGeometry(subdivison));
    }


    Ref<Plane> Plane::Create(uint32_t subdivison) 
    { return CreateRef<Plane>(subdivison); }




    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    Ref<Mesh> Plane::CreateMeshGeometry(uint32_t subdivision)
    {
        //divide by 2 means 3x3 grid
        uint32_t numX = subdivision + 1;
        uint32_t numZ = subdivision + 1;

        auto numVertices = numX * numZ;
        auto numTriangles = (numX - 1) * (numZ - 1) * 2;

        std::vector<glm::vec3> _positions(numVertices);
        std::vector <glm::vec2> _UVs(numVertices);

        glm::vec3 offset = glm::vec3(-0.5f, 0.0f, -0.5f);  //center at origin

        //j means width/x , i means height/z
        //loop row by row
        for (uint32_t i = 0; i < numZ; i++)
        {
            for (uint32_t j = 0; j < numX; j++)
            {
                _positions[j + i * numX] = glm::vec3((float)j / (numX - 1), 0.0f, (float)i / (numZ - 1)) + offset;
                _UVs[j + i * numX] = glm::vec2((float)j / (numX - 1), (float)i / (numZ - 1));
            }

        }

        // 0 1 in  0 2 3 , 0 3 1
        // 2 3
        std::vector<uint32_t> _indices(numTriangles * 3);

        for (uint32_t i = 0; i < numZ - 1; i++)
        {
            for (uint32_t j = 0; j < numX - 1; j++)
            {

                _indices[(j + i * (numX - 1)) * 6 + 0] = j + i * numX;
                _indices[(j + i * (numX - 1)) * 6 + 1] = j + (i + 1) * numX;
                _indices[(j + i * (numX - 1)) * 6 + 2] = (j + 1) + (i + 1) * numX;

                _indices[(j + i * (numX - 1)) * 6 + 3] = j + i * numX;
                _indices[(j + i * (numX - 1)) * 6 + 4] = (j + 1) + (i + 1) * numX;
                _indices[(j + i * (numX - 1)) * 6 + 5] = j + 1 + i * numX;

            }
        }


        //      // texture coordinates,  recall opengl is bottom left origin
        //      std::vector<glm::vec2> _uvs = { 
              //	glm::vec2(0.0, 1.0),
              //	glm::vec2(1.0, 1.0),
              //	glm::vec2(0.0, 0.0), 
              //	glm::vec2(1.0, 0.0)
              //};
        //    

        //      //defined in local tangent space ;  TBN is right handed, xyz respectivelyf
        //      //they are the same for all vertices 

        glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);  //y axis  world up
        glm::vec3 tangent = glm::vec3(1.0, 0.0, 0.0);  //x axis 


        //generate mesh
        auto _mesh = Mesh::Create();
        _mesh->positions = _positions;
        _mesh->UVs = _UVs;
        _mesh->normals = std::vector<glm::vec3>(_positions.size(), normal);
        _mesh->tangents = std::vector<glm::vec3>(_positions.size(), tangent);

        _mesh->indices = _indices;

        _mesh->topology = MeshTopology::TRIANGLES;
        _mesh->drawCommand = MeshDrawCommand::INDEXED;


        return _mesh;

    }














}