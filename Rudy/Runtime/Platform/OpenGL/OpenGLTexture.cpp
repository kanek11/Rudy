
#include "RudyPCH.h"

#include "OpenGLTexture.h"

namespace Rudy
{

/*   for (unsigned int i = 0; i < 6; ++i) {
   glTextureSubImage2D(cubemapTexture, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data[i]);*/

/*
   // old fashion mutable
// glBindTexture(GL_TEXTURE_2D, m_textureID);
// glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, width, height, 0, _dataFormat, _dataType, nullptr);
*/
namespace Utils
{

static GLenum TextureFormatToGLFormat(TexFormat format)
{
    switch (format)
    {
        case TexFormat::R8: return GL_R8;
        case TexFormat::R32F: return GL_R32F;
        case TexFormat::RG8: return GL_RG8;
        case TexFormat::RG32F: return GL_RG32F;
        case TexFormat::RGB8: return GL_RGB8;
        case TexFormat::RGB16F: return GL_RGB16F;
        case TexFormat::RGB32F: return GL_RGB32F;
        case TexFormat::RGBA8: return GL_RGBA8;
        case TexFormat::RGBA16F: return GL_RGBA16F;
        case TexFormat::RGBA32F: return GL_RGBA32F;

        case TexFormat::DEPTH_COMPONENT24: return GL_DEPTH_COMPONENT24;
        case TexFormat::DEPTH_COMPONENT32F: return GL_DEPTH_COMPONENT32F;
        case TexFormat::DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;

        case TexFormat::STENCIL_INDEX8: return GL_STENCIL_INDEX8;
    }

    RD_CORE_ASSERT(false);
    return 0;
}

static GLenum TextureFormatToGLDataFormat(TexFormat format)
{
    switch (format)
    {
        case TexFormat::R8: return GL_RED;
        case TexFormat::R32F: return GL_RED;
        case TexFormat::RG8: return GL_RG;
        case TexFormat::RG32F: return GL_RG;

        case TexFormat::RGB8: return GL_RGB;
        case TexFormat::RGB16F: return GL_RGB;
        case TexFormat::RGB32F: return GL_RGB;

        case TexFormat::RGBA8: return GL_RGBA;
        case TexFormat::RGBA16F: return GL_RGBA;
        case TexFormat::RGBA32F: return GL_RGBA;

        case TexFormat::DEPTH_COMPONENT24: return GL_DEPTH_COMPONENT;
        case TexFormat::DEPTH_COMPONENT32F: return GL_DEPTH_COMPONENT;
        case TexFormat::DEPTH24_STENCIL8: return GL_DEPTH_STENCIL;
    }

    RD_CORE_ASSERT(false);
    return 0;
}

static GLenum TextureInternalFormatToDataType(TexFormat format)
{
    switch (format)
    {
        case TexFormat::R8: return GL_UNSIGNED_BYTE;
        case TexFormat::R32F: return GL_FLOAT;
        case TexFormat::RG8: return GL_UNSIGNED_BYTE;
        case TexFormat::RG32F: return GL_FLOAT;

        case TexFormat::RGB8: return GL_UNSIGNED_BYTE;
        case TexFormat::RGB16F: return GL_FLOAT;
        case TexFormat::RGB32F: return GL_FLOAT;

        case TexFormat::RGBA8: return GL_UNSIGNED_BYTE;
        case TexFormat::RGBA16F: return GL_FLOAT;
        case TexFormat::RGBA32F: return GL_FLOAT;

        case TexFormat::DEPTH_COMPONENT24: return GL_UNSIGNED_INT;
        case TexFormat::DEPTH_COMPONENT32F: return GL_FLOAT;
        case TexFormat::DEPTH24_STENCIL8: return GL_UNSIGNED_INT_24_8;
    }

    RD_CORE_ASSERT(false);
    return 0;
}

static GLenum WrapModeToGLWrapMode(WrapMode mode)
{
    switch (mode)
    {
        case WrapMode::REPEAT: return GL_REPEAT;
        case WrapMode::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
        case WrapMode::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
    }

    RD_CORE_ASSERT(false);
    return 0;
}

static GLenum FilterModeToGLFilterMode(FilterMode mode)
{
    switch (mode)
    {
        case FilterMode::LINEAR: return GL_LINEAR;
        case FilterMode::NEAREST: return GL_NEAREST;
        case FilterMode::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
    }

    RD_CORE_ASSERT(false);
    return 0;
}

} // namespace Utils

void FGLTexture2D::AllocateStorage(uint32_t width, uint32_t height, TexFormat format)
{
    // RD_PROFILE_FUNCTION();

    auto _internalFormat = Utils::TextureFormatToGLFormat(format);
    auto _dataFormat     = Utils::TextureFormatToGLDataFormat(format);
    auto _dataType       = Utils::TextureInternalFormatToDataType(format);

    // immutable storage
    glTextureStorage2D(m_textureID, 1, _internalFormat, width, height);
}

void FGLTextureCube::AllocateStorage(uint32_t width, uint32_t height, TexFormat format)
{
    // RD_PROFILE_FUNCTION();

    auto _internalFormat = Utils::TextureFormatToGLFormat(format);

    glTextureStorage2D(m_textureID, 1, _internalFormat, width, height);
}

void FGLTexture2D::UploadSubData(void*     data,
                                 uint32_t  width,
                                 uint32_t  height,
                                 TexFormat format,
                                 uint32_t  level,
                                 uint32_t  xOffset,
                                 uint32_t  yOffset)
{
    // RD_PROFILE_FUNCTION();
    auto _dataFormat = Utils::TextureFormatToGLDataFormat(format);
    auto _dataType   = Utils::TextureInternalFormatToDataType(format);
    if (data)
    {
        glTextureSubImage2D(m_textureID, 0, xOffset, yOffset, width, height, _dataFormat, _dataType, data);
    }
    else
    {
        RD_CORE_ERROR("GL_TEXTURE_2D:data is nullptr");
        RD_CORE_ASSERT(false, "data is nullptr");
    }
}

//
FGLTexture2D::FGLTexture2D(const TextureConfig& texConfig)
{
    // RD_PROFILE_FUNCTION();

    m_dataFormat = Utils::TextureFormatToGLDataFormat(texConfig.texFormat);
    m_dataType   = Utils::TextureInternalFormatToDataType(texConfig.texFormat);

    auto _width          = texConfig.width;
    auto _height         = texConfig.height;
    auto _internalFormat = Utils::TextureFormatToGLFormat(texConfig.texFormat);
    auto _wrapMode       = Utils::WrapModeToGLWrapMode(texConfig.wrapMode);
    auto _minFilterMode  = Utils::FilterModeToGLFilterMode(texConfig.minFilterMode);
    auto _magFilterMode  = Utils::FilterModeToGLFilterMode(texConfig.magFilterMode);

    // deprecated old fashion
    // glGenTextures(1, &m_textureID);
    // glBindTexture(GL_TEXTURE_2D, m_textureID);
    //  glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_FLOAT, NULL);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);

    glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, _minFilterMode);
    glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, _magFilterMode);
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, _wrapMode);
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, _wrapMode);

    auto _levels = texConfig.mipLevels;
    glTextureStorage2D(m_textureID, _levels, _internalFormat, _width, _height);

    RD_CORE_INFO("GL_TEXTURE_2D: unfilled texture Id:{0} is created ", m_textureID);
    RD_CORE_INFO("storage format :{0}, width:{1}, height:{2}", _internalFormat, _width, _height);
}

FGLTexture2D::~FGLTexture2D()
{
    // RD_PROFILE_FUNCTION();
    RD_CORE_WARN("GL_TEXTURE_2D: ID:{0} is deleted ", m_textureID);
    glDeleteTextures(1, &m_textureID);
}

//===========Cube

FGLTextureCube::FGLTextureCube(const TextureConfig& texConfig)
{
    // RD_PROFILE_FUNCTION();

    m_dataFormat = Utils::TextureFormatToGLDataFormat(texConfig.texFormat);
    m_dataType   = Utils::TextureInternalFormatToDataType(texConfig.texFormat);

    auto _width          = texConfig.width;
    auto _height         = texConfig.height;
    auto _internalFormat = Utils::TextureFormatToGLFormat(texConfig.texFormat);
    auto _wrapMode       = Utils::WrapModeToGLWrapMode(texConfig.wrapMode);
    auto _minFilterMode  = Utils::FilterModeToGLFilterMode(texConfig.minFilterMode);
    auto _magFilterMode  = Utils::FilterModeToGLFilterMode(texConfig.magFilterMode);

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_textureID);

    glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, _minFilterMode);
    glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, _magFilterMode);
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, _wrapMode);
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, _wrapMode);
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_R, _wrapMode);

    /* glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
     for (uint32_t i = 0; i < 6; ++i)
     {
         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, _internalFormat, _width, _height, 0, m_dataFormat, m_dataType, NULL);
     }*/

    // modern gl recognize cubemap
    auto _levels = texConfig.mipLevels;
    glTextureStorage2D(m_textureID, _levels, _internalFormat, _width, _height);

    RD_CORE_INFO("GL_TEXTURE_CUBE_MAP: Id:{0} is created ", m_textureID);
}

FGLTextureCube::~FGLTextureCube()
{
    RD_CORE_WARN("GL_TEXTURE_CUBE_MAP: CubeMapID:{0} is deleted ", m_textureID);
    glDeleteTextures(1, &m_textureID);
}

} // namespace Rudy