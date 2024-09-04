#pragma once
#include "RudyPCH.h"

#include "Runtime/Renderer/Buffer.h"

#include <glad/glad.h>

// impose DSA principle, try to avoid state machine for configuration.
//  only if it's runtime binding.

// we donot use base class constructor. it's abstract class.
// stick to subclasses, and empty at first.

namespace Rudy
{

// be careful with diamond inheritance.
class FOpenGLBuffer : public virtual FRHIBuffer
{
public:
    FOpenGLBuffer()  = default;
    ~FOpenGLBuffer() = default;

    // inherited
public:
    virtual void  Bind() const override;
    virtual void  Unbind() const override;
    virtual void  SetData(const void* data, uint32_t size, BufferUsage usage = BufferUsage::STATIC_DRAW) override;
    virtual void* MapRange(uint32_t offset, uint32_t length) override; // access bit?
    virtual void  Unmap() override;

    virtual uint32_t GetBufferID() const override
    {
        if (m_BufferID == 0)
            RD_CORE_WARN("OpenGLBuffer:buffer id is 0");
        return m_BufferID;
    }
    virtual uint32_t GetSize() const override
    {
        if (m_Size == 0)
            RD_CORE_WARN("OpenGLBuffer:: size is 0");
        return m_Size;
    }

public:
    uint32_t m_BufferID = 0;
    uint32_t m_Size     = 0;

public:
    GLenum m_GLBufferType = 0;
};

class FOpenGLVertexBuffer : public FRHIVertexBuffer, public FOpenGLBuffer
{
public:
    ~FOpenGLVertexBuffer();
    FOpenGLVertexBuffer();
};

class FOpenGLIndexBuffer : public FRHIIndexBuffer, public FOpenGLBuffer
{
public:
    ~FOpenGLIndexBuffer();
    FOpenGLIndexBuffer();
};

class FOpenGLStorageBuffer : public RHIStorageBuffer, public FOpenGLBuffer
{
public:
    FOpenGLStorageBuffer();
    ~FOpenGLStorageBuffer();

    virtual void Bind() const override
    {
        RD_CORE_ERROR("OpenGLStorageBuffer: bind() is not standard");
    }

    virtual void BindBase(uint32_t index) const override;
};

class FOpenGLUniformBuffer : public FRHIUniformBuffer, public FOpenGLBuffer
{
public:
    FOpenGLUniformBuffer();
    ~FOpenGLUniformBuffer();

    virtual void Bind() const override
    {
        RD_CORE_ERROR("OpenGLUniformBuffer: bind() is not standard");
    }

    virtual void BindBase(uint32_t index) const override;
};

class FOpenGLVertexArray : public FRHIVertexArray
{
public:
    ~FOpenGLVertexArray();
    FOpenGLVertexArray();

    // inherited

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void AttachIndexBuffer(uint32_t buffer_id) override;
    virtual void AttachBuffer(uint32_t buffer_id, uint32_t binding_index, uint32_t stride, uint32_t offset = 0) override;

    virtual void AddAttribute(uint32_t attr_index, uint32_t binding_index, uint32_t size, BufferDataType type, uint32_t offset) override;
    virtual void AddInstancedAttribute(uint32_t attr_index, uint32_t binding_index, uint32_t size, BufferDataType type, uint32_t offset = 0, uint32_t divisor = 1) override;

    virtual uint32_t GetVertexArrayID() const override
    {
        return m_VertexArrayID;
    }

    // debug: toggle on/off attribute.
    virtual void EnableAttribute(uint32_t attri_index) override;

public:
    uint32_t                               m_VertexArrayID      = 0;
    uint32_t                               m_UsableBindingIndex = 0;
    std::unordered_map<uint32_t, uint32_t> binding_map;
    // binding_index -> buffer_id, this allows us to bind multiple, same kind of buffer by different binding points.
};

} // namespace Rudy