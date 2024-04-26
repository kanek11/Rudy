#pragma once
#include "RudyPCH.h"

#include "Rudy/Renderer/FrameBuffer.h"
#include <glad/glad.h>

namespace Rudy
{

class OpenGLRenderBuffer : public RenderBuffer
{
public:
    ~OpenGLRenderBuffer()
    {
        glDeleteRenderbuffers(1, &m_RenderBufferID);
    }

    OpenGLRenderBuffer(RenderBufferFormat format, uint32_t width, uint32_t height);

    virtual uint32_t GetRenderBufferID() override { return m_RenderBufferID; }

    static Scope<RenderBuffer> Create(RenderBufferFormat format, uint32_t width, uint32_t height);

private:
    uint32_t           m_Width = 0, m_Height = 0;
    uint32_t           m_RenderBufferID = 0;
    RenderBufferFormat m_Format         = RenderBufferFormat::DEPTH24STENCIL8;
};

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    ~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_FrameBufferID);
    }

    OpenGLFrameBuffer(std::string name, uint32_t width, uint32_t height, FrameBufferType type);

    virtual void Bind() override { glBindFramebuffer(GL_FRAMEBUFFER, this->m_FrameBufferID); }
    virtual void Unbind() override { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    virtual uint32_t GetFrameBufferID() override { return this->m_FrameBufferID; }

    virtual void SetColorTexture(TexType type, Ref<Texture> texture, uint32_t slot) override
    {
        if (slot >= 8)
        {
            RD_CORE_ERROR("FBO: colorbuffer number out of range 9!");
            return;
        }

        m_ColorBuffers[type] = texture;
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, texture->GetID(), 0);
        glNamedFramebufferTexture(this->m_FrameBufferID, GL_COLOR_ATTACHMENT0 + slot, texture->GetID(), 0);

        RD_CORE_INFO("FBO {0}: textureId:{1} is attached to colorbuffer{2}", m_Name, texture->GetID(), slot);
    }

    virtual void SetDepthTexture(Ref<Texture> texture) override
    {
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->GetID(), 0);

        m_DepthTexture = texture;
        glNamedFramebufferTexture(this->m_FrameBufferID, GL_DEPTH_ATTACHMENT, texture->GetID(), 0);
    }

    virtual std::unordered_map<TexType, Ref<Texture>> GetColorTextures() override
    {
        return m_ColorBuffers;
    }

    virtual Ref<Texture> GetDepthTexture() override
    {
        return m_DepthTexture;
    }

    virtual void SetDrawBuffers() override
    {
        // size of m_TextureBuffer
        m_ColorBufferNum = m_ColorBuffers.size();

        if (m_ColorBufferNum == 0)
        {
            RD_CORE_ERROR("FBO: no colorbuffer attrachment! be sure it's depth mapping fbo");
            glNamedFramebufferDrawBuffer(this->m_FrameBufferID, GL_NONE);
            return;
        }

        std::vector<uint32_t> colorBuffers;
        for (uint32_t slot = 0; slot < m_ColorBufferNum; slot++)
        {
            colorBuffers.push_back(GL_COLOR_ATTACHMENT0 + slot);
        }
        glNamedFramebufferDrawBuffers(this->m_FrameBufferID, m_ColorBufferNum, colorBuffers.data());
        RD_CORE_WARN("FBO {0}: {1} colorbuffers are attached.", m_Name, m_ColorBufferNum);
    }

    virtual void CheckCompleteness() override
    {
        this->Bind();
        RD_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
        this->Unbind();
    }

    virtual void FinishSetup() override
    {
        // if no depth texture, create a renderbuffer for depth/stencil
        if (m_DepthTexture == nullptr)
        {
            // depthbuffer
            m_RenderBuffer = RenderBuffer::Create(m_Width, m_Height, RenderBufferFormat::DEPTH24STENCIL8);
            glNamedFramebufferRenderbuffer(this->m_FrameBufferID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer->GetRenderBufferID());
        }

        this->SetDrawBuffers();
        this->CheckCompleteness();
    }

private:
    std::string m_Name;
    uint32_t    m_FrameBufferID  = 0;
    uint32_t    m_ColorBufferNum = 0;
    uint32_t    m_Width = 0, m_Height = 0;

    std::unordered_map<TexType, Ref<Texture>> m_ColorBuffers;
    Ref<Texture>                              m_DepthTexture;
    Scope<RenderBuffer>                       m_RenderBuffer; // only for depth/stencil
};

} // namespace Rudy