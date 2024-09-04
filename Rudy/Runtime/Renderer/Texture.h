#pragma once
#include "RudyPCH.h"

#include "Runtime/CoreFramework/Object.h"

// todo: _anisotropy support

// a data copy command. glTextureSubImage2D, but we don't find use case yet.
//

// textures has 2 sources:
// 1. load from file,  use lib like stb_image,
// the info is given as a argument,  eg: width, height, format, etc
//
// 2. generate from code,  eg: as a render target
// we will specify the info in a struct  to generate the texture

namespace Rudy
{

// standardize to be "sized"; gl calls it "internal format";
// while "data format/format" not sized;
enum class TexFormat
{
    NONE = 0,

    // colors
    R8,
    R32F,

    RG8,
    RG32F,

    RGB8,
    RGB16F,
    RGB32F,

    RGBA8,
    RGBA16F, // HDR
    RGBA32F, // render target

    // for FBO
    // standardize the depth to be 24 bit, to be compatible with default depth24_stencil8
    DEPTH_COMPONENT24,
    DEPTH_COMPONENT32F,
    DEPTH24_STENCIL8,
    STENCIL_INDEX8,

};

enum class WrapMode
{
    NONE = 0,
    REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
    MIRROR,
};

enum class FilterMode
{
    NONE = 0,
    NEAREST,
    LINEAR,
    LINEAR_MIPMAP_LINEAR,
    NEAREST_MIPMAP_NEAREST, // SSR etc

    // LinearMipmapNearest,
    // NearestMipmapLinear,
};

// better have default values;  at least width and height must be specified £º
// eg:  TextureSpec spec{ 1024, 1024 };  will use default values for other members
struct TextureConfig
{
    uint32_t  width     = 0; // intended invalid value
    uint32_t  height    = 0;
    TexFormat texFormat = TexFormat::RGB8;
    // data format in GPU is deduced from TextureInternalFormat here;
    uint32_t   mipLevels     = 1;
    bool       generateMips  = false;
    WrapMode   wrapMode      = WrapMode::REPEAT;
    FilterMode minFilterMode = FilterMode::LINEAR;
    FilterMode magFilterMode = FilterMode::LINEAR;
};

//<<abstract base>>
class FRHITexture
{
public:
    virtual ~FRHITexture() = default;

    virtual void BindToSlot(uint32_t slot = 0) const = 0;
    virtual void Unbind(uint32_t slot = 0) const     = 0;

    virtual void AllocateStorage(uint32_t width, uint32_t height, TexFormat format)                                                                           = 0;
    virtual void UploadSubData(void* data, uint32_t width, uint32_t height, TexFormat format, uint32_t level = 0, uint32_t xOffset = 0, uint32_t yOffset = 0) = 0;

    //
    uint32_t GetGPUID() const { return m_textureID; }

protected:
    uint32_t m_textureID;
};

//================================================================================================
class FRHITexture2D : public FRHITexture
{
public:
    virtual ~FRHITexture2D() = default;
};

SharedPtr<FRHITexture2D> RHICreateTexture2D(const TextureConfig&);

//================================================================================================
class FRHITextureCube : public FRHITexture
{
public:
    virtual ~FRHITextureCube() = default;
};

SharedPtr<FRHITextureCube> RHICreateTextureCube(const TextureConfig&);

//================================================================================================
//<<abstract base>>
class UTexture : public UObject
{
public:
    virtual ~UTexture();
    UTexture() = default;
    UTexture(const TextureConfig& config);

public:
    virtual uint32_t GetWidth() const;
    virtual uint32_t GetHeight() const;

    void     BindToSlot(uint32_t slot = 0);
    void     Unbind(uint32_t slot = 0);
    uint32_t GetID() const;

    bool isGrayScale() const
    {
        if (textureConfig.texFormat == TexFormat::R32F
            || textureConfig.texFormat == TexFormat::R8
            || textureConfig.texFormat == TexFormat::DEPTH_COMPONENT24)
        {
            return true;
        }
        else
            return false;
    }

public:
    TextureConfig textureConfig;

    void* data = nullptr;

public:
    virtual void InitTextureResource() = 0;

    SharedPtr<FRHITexture> GetResource() const;
    SharedPtr<FRHITexture> TextureResource = nullptr;
};

class UTexture2D : public UTexture
{
public:
    ~UTexture2D() = default;
    UTexture2D()  = default;
    UTexture2D(const TextureConfig& config) :
        UTexture(config)
    {
        InitTextureResource();
    }

    virtual void InitTextureResource() override;
};

class UTextureCube : public UTexture
{
public:
    ~UTextureCube() = default;
    UTextureCube()  = default;
    UTextureCube(const TextureConfig& config) :
        UTexture(config)
    {
        InitTextureResource();
    }

    virtual void InitTextureResource() override;
};

//================================================================================================
class TextureImporter
{
public:
    ~TextureImporter() = default;
    TextureImporter()  = default;

    SharedPtr<UTexture2D> ImportTexture2D(const std::string& path);
    // SharedPtr<UTextureCube> ImportTextureCube(const std::array<std::string, 6>& paths) { } // more flexible paths?

public:
    bool bFlipYOnLoad = true;
    bool bIsHDR       = false;
};

} // namespace Rudy
