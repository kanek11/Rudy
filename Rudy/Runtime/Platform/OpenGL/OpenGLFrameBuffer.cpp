#include "RudyPCH.h"

#include "OpenGLFrameBuffer.h"

namespace Rudy
{

// Utility function to convert formats
namespace Utils
{

static GLenum RenderBufferFormatToGLFormat(RenderBufferFormat format)
{
    switch (format)
    {
        case RenderBufferFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
        case RenderBufferFormat::DEPTH_COMPONENT24: return GL_DEPTH_COMPONENT24;
        case RenderBufferFormat::STENCIL_INDEX8: return GL_STENCIL_INDEX8;
    }

    RD_CORE_ASSERT(false, "Unknown TextureInternalFormat!");
    return 0;
}

}; // namespace Utils

OpenGLRenderBuffer::OpenGLRenderBuffer(RenderBufferFormat format, uint32_t width, uint32_t height) :
    m_Format(format), m_Width(width), m_Height(height)
{
    // create and bind renderbuffer
    glCreateRenderbuffers(1, &m_RenderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);

    // allocate storage for renderbuffer
    GLenum internalFormat = Utils::RenderBufferFormatToGLFormat(format);
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
}

OpenGLFrameBuffer::OpenGLFrameBuffer(
    std::string name, uint32_t width, uint32_t height, FrameBufferType type) :
    m_Name(name),
    m_Width(width), m_Height(height)
{
    switch (type)
    {
        case FrameBufferType::Regular:
        {
            // create and bind framebuffer
            glCreateFramebuffers(1, &m_FrameBufferID);

            RD_CORE_INFO("Regular FBO {0} ,ID:{1} is created", m_Name, m_FrameBufferID);

            break;
        }

        case FrameBufferType::DepthTexture:
        {
            // create and bind framebuffer
            glCreateFramebuffers(1, &m_FrameBufferID);

            // disable color buffer
            glNamedFramebufferDrawBuffer(m_FrameBufferID, GL_NONE);
            glNamedFramebufferReadBuffer(m_FrameBufferID, GL_NONE);

            // donot check framebuffer status

            RD_CORE_INFO("DepthTexture FBO {0} ,id:{1} is created", m_Name, m_FrameBufferID);

            break;
        }

        case FrameBufferType::GBuffer:
        {
            // create and bind framebuffer
            glCreateFramebuffers(1, &m_FrameBufferID);

            RD_CORE_INFO("Gbuffer FBO {0} ,id:{1} is created", m_Name, m_FrameBufferID);
            break;
        }
    }
}

} // namespace Rudy
