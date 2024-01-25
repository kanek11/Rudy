#include "RudyPCH.h"
#include <Rudy.h>


namespace Rudy 
{

    Ref<Mesh> Cube::CreateMeshGeometry()
    {
        //we don't use indices for now
        float CubeVertices[] = {
          
              //-x
              -1.0f, -1.0f,  1.0f,
              -1.0f, -1.0f, -1.0f,
              -1.0f,  1.0f, -1.0f,
              -1.0f,  1.0f, -1.0f,
              -1.0f,  1.0f,  1.0f,
              -1.0f, -1.0f,  1.0f,

              //+x
               1.0f, -1.0f, -1.0f,
               1.0f, -1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,

               //-z
              -1.0f,  1.0f, -1.0f,
              -1.0f, -1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,
               1.0f,  1.0f, -1.0f,
              -1.0f,  1.0f, -1.0f,
               //+z
              -1.0f, -1.0f,  1.0f,
              -1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f, -1.0f,  1.0f,
              -1.0f, -1.0f,  1.0f, 
               
              //-y
              -1.0f, -1.0f, -1.0f,
              -1.0f, -1.0f,  1.0f,
               1.0f, -1.0f, -1.0f,
               1.0f, -1.0f, -1.0f,
              -1.0f, -1.0f,  1.0f,
               1.0f, -1.0f,  1.0f, 

              //+y
              -1.0f,  1.0f, -1.0f,
               1.0f,  1.0f, -1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f,  1.0f,  1.0f,
              -1.0f,  1.0f,  1.0f,
              -1.0f,  1.0f, -1.0f,

        };

        std::vector<glm::vec3>  _positions;
        // RD_CORE_INFO("size of vertices {0}", sizeof(CubeVertices) / sizeof(float));
        // Assuming that the length of your array is a multiple of 3
        for (int i = 0; i <  sizeof(CubeVertices) / sizeof(float) - 2 ; i += 3) {
            //RD_CORE_INFO("index {0}", i+2); 
            _positions.push_back(glm::vec3(CubeVertices[i], CubeVertices[i + 1], CubeVertices[i + 2]));
        }

        //normals 
std::vector<glm::vec3> _normals = {
     
// 左面
glm::vec3(-1.0f, 0.0f, 0.0f),
glm::vec3(-1.0f, 0.0f, 0.0f),
glm::vec3(-1.0f, 0.0f, 0.0f),
glm::vec3(-1.0f, 0.0f, 0.0f),
glm::vec3(-1.0f, 0.0f, 0.0f),
glm::vec3(-1.0f, 0.0f, 0.0f),

// 右面
glm::vec3(1.0f, 0.0f, 0.0f),
glm::vec3(1.0f, 0.0f, 0.0f),
glm::vec3(1.0f, 0.0f, 0.0f),
glm::vec3(1.0f, 0.0f, 0.0f),
glm::vec3(1.0f, 0.0f, 0.0f),
glm::vec3(1.0f, 0.0f, 0.0f), 

// 后面
glm::vec3(0.0f, 0.0f, -1.0f),
glm::vec3(0.0f, 0.0f, -1.0f),
glm::vec3(0.0f, 0.0f, -1.0f),
glm::vec3(0.0f, 0.0f, -1.0f),
glm::vec3(0.0f, 0.0f, -1.0f),
glm::vec3(0.0f, 0.0f, -1.0f),

// 前面
glm::vec3(0.0f, 0.0f, 1.0f),
glm::vec3(0.0f, 0.0f, 1.0f),
glm::vec3(0.0f, 0.0f, 1.0f),
glm::vec3(0.0f, 0.0f, 1.0f),
glm::vec3(0.0f, 0.0f, 1.0f),
glm::vec3(0.0f, 0.0f, 1.0f),


// 底面
glm::vec3(0.0f, -1.0f, 0.0f),
glm::vec3(0.0f, -1.0f, 0.0f),
glm::vec3(0.0f, -1.0f, 0.0f),
glm::vec3(0.0f, -1.0f, 0.0f),
glm::vec3(0.0f, -1.0f, 0.0f),
glm::vec3(0.0f, -1.0f, 0.0f),

// 顶面
glm::vec3(0.0f, 1.0f, 0.0f),
glm::vec3(0.0f, 1.0f, 0.0f),
glm::vec3(0.0f, 1.0f, 0.0f),
glm::vec3(0.0f, 1.0f, 0.0f),
glm::vec3(0.0f, 1.0f, 0.0f),
glm::vec3(0.0f, 1.0f, 0.0f), 

};

    

        auto _mesh = Mesh::Create();
        _mesh->positions = _positions;   
        _mesh->normals = _normals;
        _mesh->topology = MeshTopology::TRIANGLES;
        _mesh->drawCommand = MeshDrawCommand::ARRAYS;

         return _mesh;


    }


 


   void Cube::DrawSkybox(Ref<Camera> cam)
   {
       auto renderer = this->GetRenderer();

       if (!renderer->hasMaterial())
       { 
           RD_CORE_WARN("Cube: no bound material");
           return;
       }
     
       if (!renderer->hasMesh())
       {
           RD_CORE_WARN("Cube: no bound mesh");
           return;
       }
    	  


      renderer->GetVertexArray()->Bind();
      renderer->GetMaterial()->Bind();
   
      glm::mat4  view = cam->GetViewMatrix();
      view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
      glm::mat4  projection = cam->GetProjectionMatrix();
   
      renderer->GetMaterial()->GetShader()->SetMat4("u_projection", projection);
      renderer->GetMaterial()->GetShader()->SetMat4("u_view", view);
   
      
     // change depth function so depth test passes when values are equal to depth buffer's content
     
      glDepthFunc(GL_LEQUAL);
      RendererApp::GetRendererAPI()->DrawArrays(MeshTopology::TRIANGLES, renderer->GetMesh()->vertices.size());
      
      glDepthFunc(GL_LESS); // set depth function back to default 
      
      
      renderer->GetVertexArray()->Unbind();
      renderer->GetMaterial()->Unbind();
    
   
   }












}