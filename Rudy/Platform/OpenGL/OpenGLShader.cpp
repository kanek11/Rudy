#include "RudyPCH.h"

#include "OpenGLShader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

namespace Rudy {


    OpenGLShader::OpenGLShader(const std::string& filepath)
        : m_FilePath(filepath)
    {

    }

    //refering to learnopengl.com
    //disable geometry shader for now.

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
        : m_Name(name)
    {
        //RD_PROFILE_FUNCTION();

        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        //std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        //std::ifstream gShaderFile;

        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        //gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            // open files
            vShaderFile.open(vertexSrc);
            fShaderFile.open(fragmentSrc);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();

            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();

            // if geometry shader path is present, also load a geometry shader
        /*    if (geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        */

        }

        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();


        // 2. compile shaders
        uint32_t vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");


        // if geometry shader is given, compile geometry shader
  /*      uint32_t geometry;
        if (geometryPath != nullptr)
        {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }*/

         
        // output of the creation.
        m_ShaderID = glCreateProgram();

        glAttachShader(m_ShaderID, vertex);
        glAttachShader(m_ShaderID, fragment);

        /* if (geometryPath != nullptr)
             glAttachShader(ID, geometry);*/

        glLinkProgram(m_ShaderID);

        checkCompileErrors(m_ShaderID, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        /*  if (geometryPath != nullptr)
              glDeleteShader(geometry);*/

        RD_CORE_INFO("OpenGLShader: shaderName: {0} is created, id:{1}", m_Name,m_ShaderID);
    }




    OpenGLShader::~OpenGLShader()
    {
        //RD_PROFILE_FUNCTION(); 
        glDeleteProgram(m_ShaderID);
    }




    // activate the shader
   // ------------------------------------------------------------------------
    void OpenGLShader::Bind() const
    {
        //RD_PROFILE_FUNCTION();

        glUseProgram(m_ShaderID); 
    }

    void OpenGLShader::Unbind() const
    {
        //RD_PROFILE_FUNCTION();

        glUseProgram(0); 
    }


    // utility uniform setter functions
    // ------------------------------------------------------------------------
    void OpenGLShader::SetBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void OpenGLShader::SetInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void OpenGLShader::SetFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_ShaderID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void OpenGLShader::SetVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, &value[0]);
    }
    void OpenGLShader::SetVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(m_ShaderID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void OpenGLShader::SetVec3(const std::string& name, const glm::vec3& value) const
    { 
        glUniform3fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, &value[0]);
    }
    void OpenGLShader::SetVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(m_ShaderID, name.c_str()), x, y, z);
    }

    // ------------------------------------------------------------------------
    void OpenGLShader::SetVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, &value[0]);
    }
    void OpenGLShader::SetVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(m_ShaderID, name.c_str()), x, y, z, w);
    }

    // ------------------------------------------------------------------------
    void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& mat) const
    {  
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            // Log error or warning
            return;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));

        //glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);

    }


    //routine utility function for checking shader compilation/linking errors.
   // ------------------------------------------------------------------------
    void OpenGLShader::checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }


}