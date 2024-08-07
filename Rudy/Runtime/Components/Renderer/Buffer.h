#pragma once
#include "RudyPCH.h"

// refer to: https://www.khronos.org/opengl/wiki/Buffer_Object
// adopt DSA since opengl 4.5:  use named version.
// assume mutable data.

// todo:
// handle copy and move ; eg: banned
//     UniformBuffer(const UniformBuffer&) = delete;
//    UniformBuffer& operator=(const UniformBuffer&) = delete;

// todo: more elegant way to handle inheritance? not is [via dominance].
namespace Rudy
{
// front end data need to by implement-independent.
enum class BufferDataType
{

    FLOAT32,
    DOUBLE64,
    INT32,
    UINT32,
    BOOL8,
    BYTE8,
    // might use high-level data type later.
    // VEC2,
    // MAT3,
    // MAT4,
};

enum class BufferType
{
    VERTEX_BUFFER,
    INDEX_BUFFER,
    SHADER_STORAGE_BUFFER,
    UNIFORM_BUFFER,
};

enum class BufferUsage
{
    STATIC_DRAW, // most vertex data, modify once by application.
    STATIC_READ,
    STATIC_COPY,
    DYNAMIC_DRAW,
    DYNAMIC_READ,
    DYNAMIC_COPY, // most compute shader data, modified by GL read.
    STREAM_DRAW,
    STREAM,
};

// void SetSubData(const void* data, uint32_t size, uint32_t offset);

// buffer object is just a chunk of memory on the GPU , the type/target doesn't technically matter.
//<<interface>> , many of the methods are generic.
class Buffer
{
public:
    virtual void Bind() const   = 0;
    virtual void Unbind() const = 0;
    // update data
    virtual void  SetData(const void* data, uint32_t size, BufferUsage usage = BufferUsage::STATIC_DRAW) = 0;
    virtual void* MapRange(uint32_t offset, uint32_t length)                                             = 0; // access bit is ignored for now.
    virtual void  Unmap()                                                                                = 0;

    virtual uint32_t GetBufferID() const = 0;
    virtual uint32_t GetSize() const     = 0;

    // public:
    //	uint32_t m_BufferID;
    //	uint32_t m_Size;
};

// todo: maybe add a "layout" to systematically specify the layout of the buffer. instead by hand.
class VertexBuffer : public virtual Buffer
{
public:
    static SharedPtr<VertexBuffer> Create();

    // uint32_t m_Stride;
    // layout info.
};

class IndexBuffer : public virtual Buffer
{
public:
    static SharedPtr<IndexBuffer> Create();
};

// for cumpute shader.
// we use "shader storage buffer" to store the data that is used by compute shader.
// in vulkun, it's also called "storage buffer".
class StorageBuffer : public virtual Buffer
{
public:
    static SharedPtr<StorageBuffer> Create();
    virtual void                    BindBase(uint32_t index) const = 0; // specify the binding point;
};

// for small, shared data among shaders.
class UniformBuffer : public virtual Buffer
{
public:
    static SharedPtr<UniformBuffer> Create();
    virtual void                    BindBase(uint32_t index) const = 0; // specify the binding point;
};

// todo: maybe maintain a attributes list.

// VAO is a state container for vertex attribute pointers. so not a buffer object per se.
// just delete unneeded methods.
//
// buffer be data source. VAO be the layout/format info.
// bind/attach buffer objects to the VAO. before add attribute.

// VAO don't need size of buffer itself, it just need the layout info.
//  when attaching:  stride = one struct size,
//  when adding attributes: offset in the struct,  size&type of attribute.
//  and divisor for "instanced attribute".

class VertexArray
{
public:
    static SharedPtr<VertexArray> Create();

public:
    virtual void Bind() const   = 0;
    virtual void Unbind() const = 0;

    virtual void AttachIndexBuffer(uint32_t buffer_id)                                                                                                             = 0;
    virtual void AttachBuffer(uint32_t buffer_id, uint32_t binding_index, uint32_t stride, uint32_t offset = 0)                                                    = 0;
    virtual void AddAttribute(uint32_t attr_index, uint32_t binding_index, uint32_t size, BufferDataType type, uint32_t offset)                                    = 0;
    virtual void AddInstancedAttribute(uint32_t attr_index, uint32_t binding_index, uint32_t size, BufferDataType type, uint32_t offset = 0, uint32_t divisor = 1) = 0;

    // get and set;
    virtual uint32_t GetVertexArrayID() const = 0;

    // debug: toggle on/off attribute.
    virtual void EnableAttribute(uint32_t attri_index) = 0;

public:
};

// uint32_t m_VertexArrayID;
// uint32_t m_UsableBindingIndex = 2;
// std::unordered_map<uint32_t, uint32_t> binding_map;
// binding_index -> buffer_id, this allows us to bind multiple, same kind of buffer by different binding points.

} // namespace Rudy