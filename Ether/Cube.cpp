#include "EtherPCH.h"
#include "Cube.h"


namespace Hazel 
{

    void Cube::CreateGeometry()
    {
        //we don't use indices for now
        float CubeVertices[] = {
              -1.0f,  1.0f, -1.0f,
              -1.0f, -1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,
               1.0f,  1.0f, -1.0f,
              -1.0f,  1.0f, -1.0f,

              -1.0f, -1.0f,  1.0f,
              -1.0f, -1.0f, -1.0f,
              -1.0f,  1.0f, -1.0f,
              -1.0f,  1.0f, -1.0f,
              -1.0f,  1.0f,  1.0f,
              -1.0f, -1.0f,  1.0f,

               1.0f, -1.0f, -1.0f,
               1.0f, -1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,

              -1.0f, -1.0f,  1.0f,
              -1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f, -1.0f,  1.0f,
              -1.0f, -1.0f,  1.0f,

              -1.0f,  1.0f, -1.0f,
               1.0f,  1.0f, -1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
              -1.0f,  1.0f,  1.0f,
              -1.0f,  1.0f, -1.0f,

              -1.0f, -1.0f, -1.0f,
              -1.0f, -1.0f,  1.0f,
               1.0f, -1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,
              -1.0f, -1.0f,  1.0f,
               1.0f, -1.0f,  1.0f
        };

        std::vector<glm::vec3>  _positions;
        // HZ_CORE_INFO("size of vertices {0}", sizeof(CubeVertices) / sizeof(float));
        // Assuming that the length of your array is a multiple of 3
        for (int i = 0; i <  sizeof(CubeVertices) / sizeof(float) - 2 ; i += 3) {
            //HZ_CORE_INFO("index {0}", i+2); 
            _positions.push_back(glm::vec3(CubeVertices[i], CubeVertices[i + 1], CubeVertices[i + 2]));
        }


        auto _mesh = Mesh::Create();
        _mesh->positions = _positions;  
        mesh = _mesh;


    }


    void Cube::Draw() 
    { 
        if (hasMaterial() )
            material->Bind();
        else
            HZ_CORE_WARN("Cube::no bound material");   

        if (hasMesh())
            mesh->Bind();
        else
            HZ_CORE_WARN("Cube::no bound mesh");

        //glm::mat4 projection_view = Renderer::GetMainCamera()->GetProjectionViewMatrix();
        //material->GetShader()->SetMat4("u_ProjectionView", projection_view);


        Renderer::GetRendererAPI()->DrawArray(mesh->vertices.size(), MeshTopology::TRIANGLES);

          
        if (hasMaterial())
            material->Unbind();
        else
            HZ_CORE_WARN("Cube::no bound material");

        if (hasMesh())
            mesh->Unbind();
        else
            HZ_CORE_WARN("Cube::no bound mesh");

       
    }





    void Cube::DrawSkybox()
    {
        if (material) 
            material->Bind();
        else
            HZ_CORE_WARN("Cube: no bound material");
      
        mesh->Bind();
 

        glm::mat4  view = Renderer::GetMainCamera()->GetViewMatrix();
        view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        glm::mat4  projection = Renderer::GetMainCamera()->GetProjectionMatrix();

       material->GetShader()->SetMat4("u_ProjectionView", projection* view);
  
       
      glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
      
      Renderer::GetRendererAPI()->DrawArray(mesh->vertices.size(), MeshTopology::TRIANGLES);

      
      glDepthFunc(GL_LESS); // set depth function back to default 
      
      
      mesh->Unbind();
  
      if (material)
          material->Unbind();
      else
          HZ_CORE_WARN("Cube: no bound material");  
  
  }












}