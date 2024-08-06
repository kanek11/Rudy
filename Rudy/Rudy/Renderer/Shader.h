#pragma once

#include "RudyPCH.h"
#include <glm/glm.hpp>

// me:
// modify the Rudy code to be more like learnopengl.com

// todo: more flexible pipeline;
// todo :make clear shader object vs shader program object;

// todo: material might accept compute shader£¿ see as i go.

namespace Rudy
{

// new:
enum class ShaderProgramType
{

    None = 0,
    // Vertex = 1 << 0,
    // Fragment = 1 << 1,
    // Geometry = 1 << 2,
    // Compute = 1 << 3

};

class Shader
{
public:
    virtual ~Shader() = default;

    virtual void Bind() const   = 0;
    virtual void Unbind() const = 0;

    virtual void SetBool(const std::string& name, bool value)   = 0;
    virtual void SetInt(const std::string& name, int value)     = 0;
    virtual void SetUInt(const std::string& name, int value)    = 0;
    virtual void SetFloat(const std::string& name, float value) = 0;

    // me: change to set Vec , instead of set float
    virtual void SetVec2(const std::string& name, const glm::vec2& value) = 0;
    virtual void SetVec2(const std::string& name, float x, float y)       = 0;

    virtual void SetIVec2(const std::string& name, const glm::ivec2& value) = 0;
    virtual void SetIVec2(const std::string& name, int x, int y)            = 0;

    virtual void SetVec3(const std::string& name, const glm::vec3& value)    = 0;
    virtual void SetVec3(const std::string& name, float x, float y, float z) = 0;

    virtual void SetVec4(const std::string& name, const glm::vec4& value)             = 0;
    virtual void SetVec4(const std::string& name, float x, float y, float z, float w) = 0;

    virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
    virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

    virtual const std::string& GetName() const     = 0;
    virtual const uint32_t     GetShaderID() const = 0;

    static SharedPtr<Shader> Create(const std::string& filepath);
    static SharedPtr<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc = "");

    // new:
    static SharedPtr<Shader> CreateComputeShader(const std::string& name, const std::string& computeSrc);
};

// to handle multiple shaders, we define a collection of shaders
// to be loaded while the application initializes
// we refer to shaders by its name

// eg:
// FlatColor;
// Metallic
// BlinnPhong

class ShaderLibrary
{
public:
    static void Add(const std::string& name, const SharedPtr<Shader>& shader);
    static void Add(const SharedPtr<Shader>& shader);
    // SharedPtr<Shader> Load(const std::string& filepath);
    // SharedPtr<Shader> Load(const std::string& name, const std::string& filepath);

    static SharedPtr<Shader> Get(const std::string& name);

    static bool Exists(const std::string& name);

    static std::unordered_map<std::string, SharedPtr<Shader>> s_shaders;
};

} // namespace Rudy
