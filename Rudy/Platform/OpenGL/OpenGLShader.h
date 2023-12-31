#pragma once


#include "Rudy/Renderer/Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>


//me: 
//not Cherno's code, we don't need translate between OpenGL and Vulkan yet.


// TODO: REMOVE!
typedef uint32_t GLenum;

namespace Rudy {

	class OpenGLShader : public Shader
	{
	public:
		
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

	 
		//====me: inherit from shader class 
		virtual void Bind() const override;
		virtual void Unbind() const override; 

		virtual void SetBool(const std::string& name, bool value)  const override;
		virtual void SetInt(const std::string& name, int value)   const override;
		virtual void SetFloat(const std::string& name, float value) const override;

		//me: change to set Vec , instead of set float
		virtual void SetVec2(const std::string& name, const glm::vec2& value) const override;
		virtual void SetVec2(const std::string& name, float x, float y)       const override;

		virtual void SetVec3(const std::string& name, const glm::vec3& value)    const override;
		virtual void SetVec3(const std::string& name, float x, float y, float z) const override;

		virtual void SetVec4(const std::string& name, const glm::vec4& value) const override;
		virtual void SetVec4(const std::string& name, float x, float y, float z, float w) const override;

		virtual void SetMat3(const std::string& name, const glm::mat3& value) const override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) const override;


		virtual const std::string& GetName() const override { return m_Name; } 
		virtual const uint32_t GetShaderID() const override { return m_ShaderID; }

 

		//utility function  
		void checkCompileErrors(GLuint shader, std::string type);


	private:
		uint32_t m_ShaderID = 0;
		std::string m_FilePath;
		std::string m_Name; 

	};


}


