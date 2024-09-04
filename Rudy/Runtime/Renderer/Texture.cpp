#include "RudyPCH.h"

#include "Texture.h"

#include "RendererAPI.h"
#include "Runtime/Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>

namespace Rudy
{

SharedPtr<FRHITexture2D> RHICreateTexture2D(const TextureConfig& texConfig)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<FGLTexture2D>(texConfig);
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<FRHITextureCube> RHICreateTextureCube(const TextureConfig& texConfig)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None: RD_CORE_ASSERT(false, "No API is set!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateShared<FGLTextureCube>(texConfig);
    }

    RD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

SharedPtr<UTexture2D> TextureImporter::ImportTexture2D(const std::string& path)
{
    stbi_set_flip_vertically_on_load(bFlipYOnLoad);

    int       _width, _height, _channels;
    TexFormat _format = TexFormat::RGBA8;

    void* tex_data = nullptr;
    if (bIsHDR)
    {
        // float*
        float* _data = stbi_loadf(path.c_str(), &_width, &_height, &_channels, 0);
        tex_data     = static_cast<void*>(_data);
        _format      = TexFormat::RGB16F;
    }
    else
    {
        stbi_uc* _data = stbi_load(path.c_str(), &_width, &_height, &_channels, 0);
        tex_data       = static_cast<void*>(_data);

        if (_channels == 3)
        {
            _format = TexFormat::RGB8;
        }
        else if (_channels == 4)
        {
            _format = TexFormat::RGBA8;
        }
        else if (_channels == 1)
        {
            _format = TexFormat::R8;
        }
        else
        {
            RD_CORE_ERROR("Unknown texture format: {0}", path);
        }
    }

    if (!tex_data)
    {
        RD_CORE_ERROR("Failed to load texture: {0}", path);
        return nullptr;
    }

    TextureConfig _config {
        .width     = static_cast<uint32_t>(_width),
        .height    = static_cast<uint32_t>(_height),
        .texFormat = _format,
    };

    auto _texture  = CreateShared<UTexture2D>(_config);
    _texture->data = static_cast<void*>(tex_data);

    RD_CORE_WARN("TEXTURE2D loaded at path:{0} ", path.c_str());
    RD_CORE_WARN("width:{0},height:{1}, channels:{2}", _width, _height, _channels);

    _texture->GetResource()->UploadSubData(tex_data, _texture->textureConfig.width, _texture->textureConfig.height, _texture->textureConfig.texFormat);

    return _texture;
}

//==

UTexture::UTexture(const TextureConfig& config) :
    UObject(), textureConfig(config)
{
}

UTexture::~UTexture()
{
    if (data)
    {
        stbi_image_free(data);
    }
}

SharedPtr<FRHITexture> UTexture::GetResource() const
{
    RD_CORE_ASSERT(TextureResource != nullptr, "TextureResource is not initialized! Lazy init!");
    return TextureResource;
}

uint32_t UTexture::GetWidth() const
{
    return textureConfig.width;
}

uint32_t UTexture::GetHeight() const
{
    return textureConfig.height;
}

void UTexture::BindToSlot(uint32_t slot)
{
    if (GetResource())
    {
        TextureResource->BindToSlot(slot);
    }
}

void UTexture::Unbind(uint32_t slot)
{
    if (GetResource())
    {
        TextureResource->Unbind(slot);
    }
}

uint32_t UTexture::GetID() const
{
    if (GetResource())
    {
        return TextureResource->GetGPUID();
    }
    return 0;
}

void UTexture2D::InitTextureResource()
{
    TextureResource = RHICreateTexture2D(textureConfig);
}

void UTextureCube::InitTextureResource()
{
    TextureResource = RHICreateTextureCube(textureConfig);
}

} // namespace Rudy