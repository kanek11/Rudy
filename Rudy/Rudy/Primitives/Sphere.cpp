#include "RudyPCH.h"

#include "Sphere.h"


namespace Rudy
{

    Sphere::Sphere(uint32_t subdivision) : StaticMeshObject(),
        m_subdivision(subdivision)
    {
        this->GetRenderer()->SetMesh(CreateMeshGeometry(subdivision));
    }

    Ref<Sphere> Sphere::Create(uint32_t subdivision)
    {
        return CreateRef<Sphere>(subdivision);
    }



    //static Scope<Mesh> Create(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    //based on spherical coordinate
    //opengl y is up,  elevation or theta,  vary from 0 to pi
    //xz plane is azimuth or phi, vary from 0 to 2pi

    //subdivision say 4 , means 4 facets; 
    //say theta varys   [0,..4]* pi/4
    Ref<Mesh> Sphere::CreateMeshGeometry(uint32_t subdivision)
    {

        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;


        const uint32_t X_SEGMENTS = subdivision;
        const uint32_t Y_SEGMENTS = subdivision;
        const float PI = 3.14159265359f;

        //x =azimuth ;y=¦È elevation
        for (uint32_t x = 0; x <= X_SEGMENTS; ++x)
        {
            for (uint32_t y = 0; y <= Y_SEGMENTS; ++y)
            {

                //normalized
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPhi = xSegment * 2.0f * PI;
                float yTheta = ySegment * PI;

                //y being up
                float yPos = std::cos(yTheta);
                float xPos = std::cos(xPhi) * std::sin(yTheta);
                float zPos = std::sin(xPhi) * std::sin(yTheta);


                Vertex vertex;

                vertex.Position = glm::vec3(xPos, yPos, zPos);
                vertex.Normal = glm::vec3(xPos, yPos, zPos);
                vertex.UV = glm::vec2(xSegment, ySegment);


                //tangent ,always on xz plane
                //as derivative to normal , dN/dphi = (-sin(phi), 0, cos(phi)) * sin(theta)

                //note at poles, sintheta = 0, tangent is undefined;
                if (yTheta == 0)
                {
                    vertex.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
                    //vertex.Bitangent = glm::vec3(0.0f, 0.0f, 1.0f); 
                }
                else if (yTheta == PI)
                {
                    vertex.Tangent = glm::vec3(-1.0f, 0.0f, 0.0f);
                }
                else
                {
                    glm::vec3 Tangent =
                        glm::vec3(-std::sin(xPhi) * std::sin(yTheta),
                            0,
                            std::cos(xPhi) * std::sin(yTheta));

                    vertex.Tangent = glm::normalize(Tangent);

                    //optional bitangent
                    //vertex.Bitangent = glm::normalize(glm::cross(glm::vec3(xPos, yPos, zPos), Tangent));
                }


                Vertices.push_back(vertex);

            }
        }

        //generate indices ; 
        // draw "strip" mode ; +2 indices for each new triangle

        for (uint32_t y = 0; y < Y_SEGMENTS; ++y)
        {
            for (uint32_t x = 0; x <= X_SEGMENTS; ++x)
            {
                Indices.push_back(y * (X_SEGMENTS + 1) + x);
                Indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }

        }

        //generate mesh
        auto _mesh = Mesh::Create();
        _mesh->vertices = Vertices;
        _mesh->indices = Indices;
        _mesh->topology = MeshTopology::STRIP;
        _mesh->drawCommand = MeshDrawCommand::INDEXED;

        return _mesh;


    }




}