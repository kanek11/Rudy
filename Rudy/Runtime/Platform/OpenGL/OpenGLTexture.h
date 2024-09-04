#pragma once

#include "RudyPCH.h"

#include "Runtime/Renderer/Texture.h"

namespace Rudy
{

class FGLTexture2D : public FRHITexture2D
{
public:
    ~FGLTexture2D();
    FGLTexture2D(const TextureConfig& specification);

    //======inherited
    virtual void BindToSlot(uint32_t slot = 0) const override
    {
        glBindTextureUnit(slot, m_textureID);
    }
    virtual void Unbind(uint32_t slot = 0) const override
    {
        glBindTextureUnit(slot, 0);
    }

    virtual void AllocateStorage(uint32_t width, uint32_t height, TexFormat format) override;
    virtual void UploadSubData(void* data, uint32_t width, uint32_t height, TexFormat format, uint32_t level, uint32_t xOffset = 0, uint32_t yOffset = 0) override;

private:
    GLenum m_dataFormat = 0;
    GLenum m_dataType   = 0;
};

class FGLTextureCube : public FRHITextureCube
{
public:
    ~FGLTextureCube();
    FGLTextureCube(const TextureConfig& specification);

    // gl utils
    virtual void BindToSlot(uint32_t slot = 0) const override
    {
        glBindTextureUnit(slot, m_textureID);
    }
    virtual void Unbind(uint32_t slot = 0) const override
    {
        glBindTextureUnit(slot, 0);
    }

    virtual void AllocateStorage(uint32_t width, uint32_t height, TexFormat format) override;

    virtual void UploadSubData(void* data, uint32_t width, uint32_t height, TexFormat texformat, uint32_t level, uint32_t xOffset = 0, uint32_t yOffset = 0) override
    {
        RD_CORE_INFO("SubData not implemented for FGLTextureCube");
    }

private:
    GLenum m_dataFormat = 0;
    GLenum m_dataType   = 0;
};

} // namespace Rudy
