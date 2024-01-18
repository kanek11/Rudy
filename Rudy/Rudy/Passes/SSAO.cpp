#include "RudyPCH.h"   
 
#include "SSAO.h"

namespace  Rudy
{

	Ref<SSAO> SSAO::Create(
		uint32_t m_width, uint32_t m_height,
		std::map< TextureType, Ref<Texture2D>> m_ssaoInputs,
		std::map< TextureType, Ref<Texture2D>> m_ssaoOutputs)
	{
		return CreateRef<SSAO>( m_width,  m_height, m_ssaoInputs, m_ssaoOutputs);
	}

	SSAO::SSAO(
		uint32_t m_width, uint32_t m_height,
		std::map< TextureType, Ref<Texture2D>> m_ssaoInputs,
		std::map< TextureType, Ref<Texture2D>> m_ssaoOutputs)
	 :m_width(m_width), m_height(m_height), 
	 m_ssaoInputs(m_ssaoInputs), m_ssaoOutputs(m_ssaoOutputs)
	{
		Init();
	}



    //material: ssao takes worldpos, worldnormal, 
    //make sure use a consistent space for the depth;  
    // custom sampling kernel, rotation noise texture 

    //refer to https://learnopengl.com/Advanced-Lighting/SSAO

    void SSAO::Init()
    {

        ssaoShader = Shader::Create("ssao Shader", "Shaders/PostProcess/SSAO_VS.glsl", "Shaders/PostProcess/SSAO_FS.glsl");
        Material::SetMaterialProperties(ssaoShader); 

        ssaoFBO = FrameBuffer::Create(" ssao FBO",
            m_width, m_height, FrameBufferType::Regular); 


        ssaoFBO->SetColorTexture(TextureType::ScreenTexture, m_ssaoOutputs[TextureType::ScreenTexture], 0);
        ssaoFBO->FinishSetup();
         


        auto ssaoMaterial = Material::Create(ssaoShader);
        ssaoMaterial->SetTexture(TextureType::gPosition,     m_ssaoInputs[TextureType::gPosition]);
        ssaoMaterial->SetTexture(TextureType::gWorldNormal,  m_ssaoInputs[TextureType::gWorldNormal ]);
        ssaoMaterial->SetTexture(TextureType::gWorldTangent, m_ssaoInputs[TextureType::gWorldTangent ]);
        ssaoMaterial->SetTexture(TextureType::gScreenDepth,  m_ssaoInputs[TextureType::gScreenDepth]);



        auto m_Lerp = [](float a, float b, float f) -> float {
            return a + f * (b - a);   };

        std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
        std::default_random_engine generator;
        std::vector<glm::vec3> ssaoKernel;
        for (unsigned int i = 0; i < 64; ++i)
        {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 64.0f;

            // scale samples s.t. they're more aligned to center of kernel
            scale = m_Lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }


        //used by all fragments;
        ssaoShader->Bind();
        for (unsigned int i = 0; i < 64; ++i)
            ssaoShader->SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);


        // generate noise texture
        // ----------------------
        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            ssaoNoise.push_back(noise);
        }

        auto ssaoNoiseTexture = Texture2D::CreateEmpty(TextureSpec{
                4, 4, TextureInternalFormat::RGB32F,
                false, WrapMode::Repeat, FilterMode::Nearest });

        ssaoNoiseTexture->SubData(ssaoNoise.data(), 4, 4);



        ssaoMaterial->SetTexture(TextureType::NoiseTexture, ssaoNoiseTexture);


        //-------the quad for ssao pass;
       // Quad ssaoQuad = Quad();
        ssaoQuad = ScreenQuad::Create();
        ssaoQuad->SetMaterial(ssaoMaterial);

 


    }
     

	void SSAO::Render(Ref<Camera> main_camera)
	{
         
        ssaoShader->SetMat4("u_View", main_camera->GetViewMatrix());
        ssaoShader->SetMat4("u_Projection", main_camera->GetProjectionMatrix());


        ssaoFBO->Bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, m_width, m_height); 
     

        ssaoQuad->Draw(nullptr);

        ssaoFBO->Unbind();

	}











}