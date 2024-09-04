#include "RudyPCH.h"

#include "PrecomputeCubemapPass.h"

#include "Runtime/Actors/Primitives/Cube.h"

namespace Rudy
{

glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
glm::mat4 captureViews[]    = {
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};

SharedPtr<UTextureCube> PrecomputeCubemapPass::RecToCube(const SharedPtr<UTexture2D>& recTexture)
{
    // output
    auto out_cubemap = NewObject<UTextureCube>(
        "rectocubemap",
        TextureConfig {
            .width     = output_height,
            .height    = output_width,
            .texFormat = recTexture->textureConfig.texFormat,
        });

    auto rectToCubeShader = Shader::Create("equirectangular to Cubemap Shader",
                                           "Shaders/Shaders/CubemapCapture_VS.glsl",
                                           "Shaders/Shaders/RectToCube_FS.glsl",
                                           "Shaders/Shaders/CubemapCapture_GS.glsl");

    // material
    auto rectToCubeMaterial = Material::Create("rectToCubeMaterial");
    rectToCubeMaterial->SetTexture(TexType::EnvironmentMap, recTexture);

    auto _captureCube  = CreateActor<Cube>();
    _captureCube->Name = "recToCube";
    _captureCube->SetShader(rectToCubeShader);
    _captureCube->SetMaterial(rectToCubeMaterial);

    uint32_t captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, out_cubemap->GetID(), 0);

    // check validity of framebuffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        RD_CORE_ERROR("Framebuffer is not complete!");
        RD_CORE_ASSERT(false, "Framebuffer is not complete!");
    }

    rectToCubeShader->Bind();
    rectToCubeShader->SetMat4("u_projection", captureProjection);

    for (uint32_t i = 0; i < 6; ++i)
    {
        rectToCubeShader->SetMat4("u_viewMatrices[" + std::to_string(i) + "]", captureViews[i]);
    }

    glClearColor(1.0f, 0.01f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, output_height, output_height);

    _captureCube->StaticMeshComponent->SetGeometryState(rectToCubeShader);
    _captureCube->StaticMeshComponent->SetMaterialState(rectToCubeShader);
    _captureCube->StaticMeshComponent->Draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return out_cubemap;
}

SharedPtr<UTextureCube> PrecomputeCubemapPass::SpecularPrefilter(const SharedPtr<UTextureCube>& envMap)
{
    uint32_t mipLevels = 5;

    // the output prefilter Cube map;
    auto out_cubemap = NewObject<UTextureCube>(
        "prefilterEnvMap",
        TextureConfig {
            .width         = output_width,
            .height        = output_height,
            .texFormat     = TexFormat::RGB32F,
            .mipLevels     = mipLevels,
            .generateMips  = false,
            .wrapMode      = WrapMode::CLAMP_TO_EDGE,
            .minFilterMode = FilterMode::LINEAR_MIPMAP_LINEAR,
            .magFilterMode = FilterMode::LINEAR });

    auto prefilterShader = Shader::Create(
        "Prefilter specular Shader",
        "Shaders/Shaders/CubemapCapture_VS.glsl",
        "Shaders/Shaders/SpecularEnvMap_FS.glsl",
        "Shaders/Shaders/CubemapCapture_GS.glsl");

    auto CubeMaterial = Material::Create("prefilterMaterial");
    CubeMaterial->SetTexture(TexType::EnvironmentMap, envMap);

    // the background Cube;
    auto _captureCube = CreateActor<Cube>();
    _captureCube->SetMaterial(CubeMaterial);
    _captureCube->SetShader(prefilterShader);

    // framebuffer
    uint32_t captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, out_cubemap->GetID(), 0);

    // check validity of framebuffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        RD_CORE_ERROR("Framebuffer is not complete!");
        RD_CORE_ASSERT(false, "Framebuffer is not complete!");
    }

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (Cube)map.
    // ----------------------------------------------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    // uint32_t maxMipLevels = 5;
    for (uint32_t mip = 0; mip < mipLevels; ++mip)
    {
        // std::cout << mip << std::endl;
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, out_cubemap->GetID(), static_cast<int>(mip));

        // reisze framebuffer according to mip-level size.
        uint32_t mipWidth  = static_cast<uint32_t>(output_height * std::pow(0.5, mip));
        uint32_t mipHeight = static_cast<uint32_t>(output_height * std::pow(0.5, mip));
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / ((float)mipLevels - 1);

        prefilterShader->Bind();
        prefilterShader->SetFloat("u_Roughness", roughness);

        prefilterShader->SetMat4("u_projection", captureProjection);
        for (uint32_t i = 0; i < 6; ++i)
        {
            prefilterShader->SetMat4("u_viewMatrices[" + std::to_string(i) + "]", captureViews[i]);
        }

        glClearColor(1.0f, 0.01f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        _captureCube->StaticMeshComponent->SetGeometryState(prefilterShader);
        _captureCube->StaticMeshComponent->SetMaterialState(prefilterShader);
        _captureCube->StaticMeshComponent->Draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return out_cubemap;
}

} // namespace Rudy