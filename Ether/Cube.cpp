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



        glGenVertexArrays(1, &CubeVAO);
        glGenBuffers(1, &CubeVBO);

        glBindVertexArray(CubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), &CubeVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


        HZ_CORE_INFO("cube: CubeVAO: {0} is created", CubeVAO);

    }


    void Cube::Draw()

    {


        if (m_Material == nullptr)
            HZ_CORE_WARN("Cube::no bound material");
        else
            m_Material->Bind();

        glBindVertexArray(CubeVAO);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);

        if (m_Material == nullptr)
            HZ_CORE_WARN("Cube::no bound material");
        else
            m_Material->Unbind();
    }


    void Cube::DrawSkybox()
    {
        if (m_Material == nullptr)
            HZ_CORE_WARN("Cube::no bound material");
        else
            m_Material->Bind();

        glBindVertexArray(CubeVAO);

        ////auto view = glm::mat4(glm::mat3(Renderer::GetMainCamera()->GetViewMatrix())) ;  //error: undefined behaviour.
        //auto view = Renderer::GetMainCamera()->GetViewMatrix();
        //view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
        //auto projection = Renderer::GetMainCamera()->GetProjectionMatrix();
        //skyboxShader->SetMat4("u_View", view ); // remove translation from the view matrix
        //skyboxShader->SetMat4("u_Projection", projection);


        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content


        glDrawArrays(GL_TRIANGLES, 0, 36);



        glBindVertexArray(0);
        if (m_Material == nullptr)
            HZ_CORE_WARN("Cube::no bound material");
        else
            m_Material->Unbind();


        glDepthFunc(GL_LESS); // set depth function back to default

    }












}