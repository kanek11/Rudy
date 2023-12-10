#include "RudyPCH.h"
#include "Cube.h"


namespace Rudy 
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
        // RD_CORE_INFO("size of vertices {0}", sizeof(CubeVertices) / sizeof(float));
        // Assuming that the length of your array is a multiple of 3
        for (int i = 0; i <  sizeof(CubeVertices) / sizeof(float) - 2 ; i += 3) {
            //RD_CORE_INFO("index {0}", i+2); 
            _positions.push_back(glm::vec3(CubeVertices[i], CubeVertices[i + 1], CubeVertices[i + 2]));
        }


        auto _mesh = Mesh::Create();
        _mesh->positions = _positions;  
        mesh = _mesh;


    }


    void Cube::Draw(Ref<Camera> camera)
    { 
        if (hasMaterial() )
            material->Bind();
        else
            RD_CORE_WARN("Cube::no bound material");   

        if (hasMesh())
            mesh->Bind();
        else
            RD_CORE_WARN("Cube::no bound mesh");

        if (!camera)
        {
            //RD_CORE_WARN("Cube::no camera specified"); 
        } 
        else
        { 
            glm::mat4 model = this->transform->GetWorldTransform();
            material->GetShader()->SetMat4("u_Model", model);
            
            glm::mat4 projection_view = camera->GetProjectionViewMatrix();
            auto shader = material->GetShader(); 
            shader->SetMat4("u_ProjectionView", projection_view); 

        }



        Renderer::GetRendererAPI()->DrawArray(mesh->vertices.size(), MeshTopology::TRIANGLES);

          
        if (hasMaterial())
            material->Unbind();
        else
            RD_CORE_WARN("Cube::no bound material");

        if (hasMesh())
            mesh->Unbind();
        else
            RD_CORE_WARN("Cube::no bound mesh");

       
    }





    void Cube::DrawSkybox()
    {
        if (material) 
            material->Bind();
        else
            RD_CORE_WARN("Cube: no bound material");
      
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
          RD_CORE_WARN("Cube: no bound material");  
  
  }












}