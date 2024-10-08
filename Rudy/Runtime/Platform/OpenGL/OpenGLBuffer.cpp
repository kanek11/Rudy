#pragma once
#include "RudyPCH.h"
#include "OpenGLBuffer.h"

namespace Rudy
{
namespace Utils
{

static GLenum BufferDataTypeToGLType(BufferDataType type)
{
    switch (type)
    {
        case BufferDataType::FLOAT32: return GL_FLOAT;
        case BufferDataType::DOUBLE64: return GL_DOUBLE;
        case BufferDataType::INT32: return GL_INT;
        case BufferDataType::UINT32: return GL_UNSIGNED_INT;
        case BufferDataType::BOOL8: return GL_BOOL;
        case BufferDataType::BYTE8: return GL_BYTE;
        default:
            RD_CORE_WARN("Unknown BufferDataType");
            break;
    }
    return 0;
}

// static GLenum BufferTypeToGLType(BufferType type)
// {
//     switch (type)
//     {
//     case BufferType::VERTEX_BUFFER: return GL_ARRAY_BUFFER;
//     case BufferType::INDEX_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
//     case BufferType::SHADER_STORAGE_BUFFER: return GL_SHADER_STORAGE_BUFFER;
//     case BufferType::UNIFORM_BUFFER: return GL_UNIFORM_BUFFER;
//     default:
//         RD_CORE_WARN("Unknown BufferType");
//         break;
//     }
//     return 0;
// }

static GLenum BufferUsageToGLUsage(BufferUsage usage)
{
    switch (usage)
    {
        case BufferUsage::STATIC_DRAW: return GL_STATIC_DRAW;
        case BufferUsage::STATIC_READ: return GL_STATIC_READ;
        case BufferUsage::STATIC_COPY: return GL_STATIC_COPY;
        case BufferUsage::DYNAMIC_DRAW: return GL_DYNAMIC_DRAW;
        case BufferUsage::DYNAMIC_READ: return GL_DYNAMIC_READ;
        case BufferUsage::DYNAMIC_COPY: return GL_DYNAMIC_COPY;
        case BufferUsage::STREAM_DRAW: return GL_STREAM_DRAW;
        case BufferUsage::STREAM:
            return GL_STREAM_DRAW; // 假设 STREAM 对应 STREAM_DRAW
            // 添加其他案例...
        default:
            RD_CORE_WARN("Unknown BufferUsage");
            break;
    }
    return 0;
}

} // namespace Utils

void FOpenGLBuffer::Bind() const
{
    glBindBuffer(m_GLBufferType, m_BufferID);
}

void FOpenGLBuffer::Unbind() const
{
    glBindBuffer(m_GLBufferType, 0);
}

void FOpenGLBuffer::SetData(const void* data, uint32_t size, BufferUsage usage)
{
    auto GLUsage = Utils::BufferUsageToGLUsage(usage);

    glNamedBufferData(m_BufferID, size, data, GLUsage);
    m_Size = size;
}

void* FOpenGLBuffer::MapRange(uint32_t offset, uint32_t length)
{
    return glMapNamedBufferRange(m_BufferID, offset, length, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
}

void FOpenGLBuffer::Unmap()
{
    glUnmapNamedBuffer(m_BufferID);
}

FOpenGLVertexBuffer::FOpenGLVertexBuffer()
{
    m_GLBufferType = GL_ARRAY_BUFFER;
    glCreateBuffers(1, &m_BufferID);
    RD_CORE_INFO("GLVertexBuffer created ID = {0}", m_BufferID);
}

FOpenGLVertexBuffer::~FOpenGLVertexBuffer()
{
    glDeleteBuffers(1, &m_BufferID);

    RD_CORE_WARN("GLVertexBuffer deleted ID = {0}", m_BufferID);
}

FOpenGLIndexBuffer::FOpenGLIndexBuffer()
{
    m_GLBufferType = GL_ELEMENT_ARRAY_BUFFER;
    glCreateBuffers(1, &m_BufferID);
    RD_CORE_INFO("GLIndexBuffer created ID = {0}", m_BufferID);
}

FOpenGLIndexBuffer::~FOpenGLIndexBuffer()
{
    glDeleteBuffers(1, &m_BufferID);
    RD_CORE_WARN("GLIndexBuffer deleted ID = {0}", m_BufferID);
}

//=================================================================================================
FOpenGLStorageBuffer::FOpenGLStorageBuffer()
{
    m_GLBufferType = GL_SHADER_STORAGE_BUFFER;
    glCreateBuffers(1, &m_BufferID);
    RD_CORE_INFO("GLStorageBuffer created ID = {0}", m_BufferID);
}

FOpenGLStorageBuffer::~FOpenGLStorageBuffer()
{
    glDeleteBuffers(1, &m_BufferID);
    RD_CORE_WARN("GLStorageBuffer deleted ID = {0}", m_BufferID);
}

void FOpenGLStorageBuffer::BindBase(uint32_t index) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_BufferID);
}

//=================================================================================================
FOpenGLUniformBuffer::FOpenGLUniformBuffer()
{
    m_GLBufferType = GL_UNIFORM_BUFFER;
    glCreateBuffers(1, &m_BufferID);
    RD_CORE_INFO("GLUniformBuffer created ID = {0}", m_BufferID);
}

FOpenGLUniformBuffer::~FOpenGLUniformBuffer()
{
    glDeleteBuffers(1, &m_BufferID);
    RD_CORE_WARN("GLUniformBuffer deleted ID = {0}", m_BufferID);
}

void FOpenGLUniformBuffer::BindBase(uint32_t index) const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, index, m_BufferID);
}

//=================================================================================================
FOpenGLVertexArray::FOpenGLVertexArray()
{
    glCreateVertexArrays(1, &m_VertexArrayID);
    RD_CORE_INFO("GLVertexArray created ID = {0}", m_VertexArrayID);
    m_UsableBindingIndex = 3;
}

FOpenGLVertexArray::~FOpenGLVertexArray()
{
    glDeleteVertexArrays(1, &m_VertexArrayID);
    RD_CORE_WARN("GLVertexArray deleted ID = {0}", m_VertexArrayID);
}

void FOpenGLVertexArray::Bind() const
{
    glBindVertexArray(m_VertexArrayID);
}

void FOpenGLVertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void FOpenGLVertexArray::EnableAttribute(uint32_t index)
{
    glEnableVertexArrayAttrib(m_VertexArrayID, index);
}

void FOpenGLVertexArray::AttachIndexBuffer(uint32_t buffer_id)
{
    glVertexArrayElementBuffer(m_VertexArrayID, buffer_id);
}

void FOpenGLVertexArray::AttachBuffer(uint32_t buffer_id, uint32_t binding_index, uint32_t stride, uint32_t offset)
{
    glVertexArrayVertexBuffer(m_VertexArrayID, binding_index, buffer_id, offset, stride);
    // if that slot already has a binding,  throw warn
    if (binding_map.find(binding_index) != binding_map.end())
        RD_CORE_WARN("GLVertexArray::AttachBuffer: binding_index {0} already has a binding", binding_index);
    binding_map[binding_index] = buffer_id;
}

void FOpenGLVertexArray::AddAttribute(uint32_t attri_index, uint32_t binding_index, uint32_t stride_size, BufferDataType type, uint32_t offset)
{
    glEnableVertexArrayAttrib(m_VertexArrayID, attri_index);
    glVertexArrayAttribFormat(m_VertexArrayID, attri_index, stride_size, Utils::BufferDataTypeToGLType(type), false, offset);
    glVertexArrayAttribBinding(m_VertexArrayID, attri_index, binding_index);
}

void FOpenGLVertexArray::AddInstancedAttribute(uint32_t attri_index, uint32_t binding_index, uint32_t stride_size, BufferDataType type, uint32_t offset, uint32_t divisor)
{
    glEnableVertexArrayAttrib(m_VertexArrayID, attri_index);
    glVertexArrayAttribFormat(m_VertexArrayID, attri_index, stride_size, Utils::BufferDataTypeToGLType(type), false, offset);
    glVertexArrayAttribBinding(m_VertexArrayID, attri_index, binding_index);
    glVertexArrayBindingDivisor(m_VertexArrayID, binding_index, divisor);
}

} // namespace Rudy