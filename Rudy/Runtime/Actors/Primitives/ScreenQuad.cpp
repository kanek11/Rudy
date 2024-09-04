#include "RudyPCH.h"

#include "ScreenQuad.h"

namespace Rudy
{

ScreenQuad::ScreenQuad() :
    AStaticMeshActor()
{
    StaticMeshComponent->SetMesh(CreateMeshGeometry());
}

void ScreenQuad::DrawSelfContained()
{
    auto _shader = StaticMeshComponent->GetShader();
    RD_ASSERT(_shader != nullptr, "No shader attached to the mesh component");

    _shader->Bind();

    StaticMeshComponent->SetGeometryState(_shader);
    StaticMeshComponent->SetMaterialState(_shader);

    StaticMeshComponent->Draw();
}

SharedPtr<UMesh> ScreenQuad::CreateMeshGeometry()
{
    auto _mesh         = UMesh::Create();
    _mesh->topology    = MeshTopology::TRIANGLES;
    _mesh->drawCommand = MeshDrawCommand::INDEXED;
    _mesh->indices     = { 0, 1, 2, 1, 3, 2 };

    // float quadVertices[] = {
    //     // positions        // texture Coords
    //     -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
    //     -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
    //      1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
    //      1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    // };

    _mesh->positions = {
        glm::vec3(-1.0f, 1.0f, 0.0f),
        glm::vec3(-1.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, 0.0f)
    };

    _mesh->UVs = {

        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f)

    };

    return _mesh;
}

} // namespace Rudy