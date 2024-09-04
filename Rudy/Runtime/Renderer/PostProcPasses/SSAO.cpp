#include "RudyPCH.h"

#include "SSAO.h"

namespace Rudy
{

SSAO::SSAO(
    uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSAOInputs, std::unordered_map<TexType, SharedPtr<UTexture>>& m_SSAOOutputs) :
    m_width(m_width),
    m_height(m_height),
    m_SSAOInputs(m_SSAOInputs), m_SSAOOutputs(m_SSAOOutputs)
{
    Init();
}

// material: SSAO takes worldpos, worldnormal,
// make sure use a consistent space for the depth;
//  custom sampling kernel, rotation noise texture

// refer to https://learnopengl.com/Advanced-Lighting/SSAO

void SSAO::Init()
{
    SSAOShader = Shader::Create("SSAO Shader",
                                "Shaders/PostProcess/SSAO_VS.glsl",
                                "Shaders/PostProcess/SSAO_FS.glsl");
    SSAOFBO    = FrameBuffer::Create(" SSAO FBO",
                                  m_width,
                                  m_height,
                                  FrameBufferType::Regular);

    SSAOOnlyTex = NewObject<UTexture2D>("", TextureConfig { m_width, m_height, TexFormat::R32F, 1, false, WrapMode::CLAMP_TO_EDGE, FilterMode::NEAREST });

    SSAOBlurTexture = NewObject<UTexture2D>("",
                                            TextureConfig { m_width, m_height, TexFormat::R32F, 1, false, WrapMode::CLAMP_TO_BORDER, FilterMode::NEAREST });

    m_SSAOOutputs[TexType::ScreenTexture] = SSAOBlurTexture;

    SSAOFBO->SetColorTexture(TexType::ScreenTexture, SSAOOnlyTex, 0);
    SSAOFBO->FinishSetup();

    SSAOMaterial = Material::Create("SSAOMaterial");
    SSAOMaterial->SetTextures(m_SSAOInputs);

    std::unordered_map<std::string, float> FloatMap = {
        { "u_radius", radius },
        { "u_bias", bias },
    };

    SSAOMaterial->SetFloatMap(FloatMap);

    auto m_Lerp = [](float a, float b, float f) -> float
    { return a + f * (b - a); };

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::mt19937                            generator(std::random_device {}());
    // random_device rd;  //generate true random seed for the random engine;
    // thend sample the distribution;

    std::vector<glm::vec3> SSAOKernel;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 0.99 + 0.01); // reduce self-occlusion of planes.
        // randomFloats(generator));

        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = m_Lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        SSAOKernel.push_back(sample);
        // std::cout << sample.x << " " << sample.y << " " << sample.z << std::endl;
    }

    SSAOShader->Bind();
    // used by all fragments;
    for (unsigned int i = 0; i < 64; ++i)
        SSAOShader->SetVec3("samples[" + std::to_string(i) + "]", SSAOKernel[i]);

    // generate noise texture
    // ----------------------
    std::vector<glm::vec3> SSAONoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0,
                        randomFloats(generator) * 2.0 - 1.0,
                        0.0f); // rotate around z-axis (in tangent space)
        SSAONoise.push_back(noise);
    }

    auto SSAONoiseTexture = NewObject<UTexture2D>("",
                                                  TextureConfig { 4, 4, TexFormat::RGB32F, 1, false, WrapMode::REPEAT, FilterMode::NEAREST });

    SSAONoiseTexture->GetResource()->UploadSubData(SSAONoise.data(), 4, 4, SSAONoiseTexture->textureConfig.texFormat);

    SSAOMaterial->SetTexture(TexType::NoiseTexture, SSAONoiseTexture);

    //-------the quad for SSAO pass;
    // Quad SSAOQuad = Quad();
    SSAOQuad = CreateActor<ScreenQuad>();
    SSAOQuad->SetShader(SSAOShader);
    SSAOQuad->SetMaterial(SSAOMaterial);

    // blur
    BlurShader = Shader::CreateComputeShader("Blur Shader",
                                             //"Shaders/PostProcess/GaussianBlur_CS.glsl");
                                             "Shaders/PostProcess/averageBlur_CS.glsl");
}

void SSAO::Render(const SharedPtr<ACameraActor>& camera)
{
    SSAOShader->Bind();
    // SSAOShader->SetMat4("u_view",       main_camera->GetViewMatrix());
    SSAOShader->SetMat4("u_projection", camera->CameraComponent->GetProjectionMatrix());

    SSAOMaterial->SetFloat("u_radius", radius);
    SSAOMaterial->SetFloat("u_bias", bias);

    SSAOMaterial->SetFloat("u_range_bias", range_bias);

    SSAOShader->SetInt("u_kernelSize", kernelSize);

    SSAOFBO->Bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, m_width, m_height);

    SSAOQuad->DrawSelfContained();
    SSAOShader->Unbind();

    SSAOFBO->Unbind();

    // blur
    if (enableBlur)
    {
        m_SSAOOutputs[TexType::ScreenTexture] = SSAOBlurTexture;
        BlurShader->Bind();
        BlurShader->SetInt("u_radius", ave_radius);
        // BlurShader->SetFloat("u_sigma", 7.0f);

        glBindImageTexture(0, SSAOOnlyTex->GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        glBindImageTexture(1, m_SSAOOutputs[TexType::ScreenTexture]->GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

        glDispatchCompute(m_width / 8, m_height / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        BlurShader->Unbind();
    }
    else
    {
        m_SSAOOutputs[TexType::ScreenTexture] = SSAOOnlyTex;
    }
}

} // namespace Rudy