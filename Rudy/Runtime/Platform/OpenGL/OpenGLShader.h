#pragma once
#include "RudyPCH.h"

#include "Runtime/Renderer/Shader.h"

// me:
namespace Rudy
{

// info for uniform variable in shader
struct UniformInfo
{
    std::string name;
    uint32_t    location;
};

class OpenGLShader : public Shader
{
public:
    ~OpenGLShader();
    OpenGLShader(const std::string& filepath); // load from single file, not used

    OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc);
    // geometry shader is optional, default is empty() in front end

    OpenGLShader(const std::string& name, const std::string& computeSrc);

    //====inherit
    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetBool(const std::string& name, bool value) override;
    virtual void SetInt(const std::string& name, int value) override;
    virtual void SetUInt(const std::string& name, int value) override;
    virtual void SetFloat(const std::string& name, float value) override;

    // me: change to set Vec , instead of set float
    virtual void SetVec2(const std::string& name, const glm::vec2& value) override;
    virtual void SetVec2(const std::string& name, float x, float y) override;

    virtual void SetIVec2(const std::string& name, const glm::ivec2& value) override;
    virtual void SetIVec2(const std::string& name, int x, int y) override;

    virtual void SetVec3(const std::string& name, const glm::vec3& value) override;
    virtual void SetVec3(const std::string& name, float x, float y, float z) override;

    virtual void SetVec4(const std::string& name, const glm::vec4& value) override;
    virtual void SetVec4(const std::string& name, float x, float y, float z, float w) override;

    virtual void SetMat3(const std::string& name, const glm::mat3& value) override;
    virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

    virtual const std::string& GetName() const override { return m_Name; }
    virtual const uint32_t     GetShaderID() const override { return m_ShaderID; }

    //========utility
    void checkCompileErrors(GLuint shader, std::string type);

private:
    uint32_t    m_ShaderID = 0;
    std::string m_FilePath;
    std::string m_Name;

public:
    // new:
    std::map<std::string, GLint> m_uniformLocationCache;
    void                         CacheLocation(const std::string& name);
    GLint                        GetLocation(const std::string& name);
};

} // namespace Rudy
