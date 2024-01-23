#include "RudyPCH.h"

#include "OpenGLShader.h"
 

namespace Rudy {


    OpenGLShader::OpenGLShader(const std::string& filepath)
        : m_FilePath(filepath)
    { 
    }


    OpenGLShader::OpenGLShader(const std::string& name, const std::string& computeSrc)
        : m_Name(name)
    {

//RD_PROFILE_FUNCTION();
         
		std::string computeCode; 
		std::ifstream cShaderFile;
         
		cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        { 
            cShaderFile.open(computeSrc);
            std::stringstream cShaderStream; 
            cShaderStream << cShaderFile.rdbuf(); 
            cShaderFile.close(); 
            computeCode = cShaderStream.str();
        }

        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::CUMPUTE SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            std::cout << "Exception message: " << e.what() << std::endl;

        }
         

       const char* cShaderCode = computeCode.c_str();
 
		uint32_t compute; 
		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &cShaderCode, NULL);
		glCompileShader(compute);
		checkCompileErrors(compute, "COMPUTE");
         
		m_ShaderID = glCreateProgram(); 
		glAttachShader(m_ShaderID, compute); 
		glLinkProgram(m_ShaderID); 
		checkCompileErrors(m_ShaderID, "PROGRAM");
 
        glDeleteShader(compute);

		RD_CORE_INFO("OpenGLShader: compute shaderName: {0} is created, ID:{1}", m_Name, m_ShaderID);

         
    }

 
    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc,
        const std::string& fragmentSrc,
        const std::string& geometrySrc)
        : m_Name(name)
    {
        //RD_PROFILE_FUNCTION();

        // 1. retrieve the source code from filePath,  just strings.
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;

        //ifstream: input file stream
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;

        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

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
            if (!geometrySrc.empty())
            {
                gShaderFile.open(geometrySrc);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        

        }

        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            std::cout << "Exception message: " << e.what() << std::endl;

        }

        //cast into C-string that OpenGL accepts
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


 
        uint32_t geometry;
        if (!geometrySrc.empty())
        {
            RD_CORE_WARN("OpenGLShader: geometrySrc exist");
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        } 

         
        // output of the creation.
        m_ShaderID = glCreateProgram();

        glAttachShader(m_ShaderID, vertex);
        glAttachShader(m_ShaderID, fragment);

        if (!geometrySrc.empty())
             glAttachShader(m_ShaderID, geometry);

        glLinkProgram(m_ShaderID);

        checkCompileErrors(m_ShaderID, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        if (!geometrySrc.empty())
              glDeleteShader(geometry);

        RD_CORE_INFO("OpenGLShader: shaderProgram Name: {0} is created, ID:{1}", m_Name,m_ShaderID);
    }




    OpenGLShader::~OpenGLShader()
    {
        //RD_PROFILE_FUNCTION(); 
        RD_CORE_INFO("OpenGLShader: shaderName: {0} is deleted, ID:{1}", m_Name, m_ShaderID);
        glDeleteProgram(m_ShaderID);
    }




    // activate the shader
   // ------------------------------------------------------------------------
    void OpenGLShader::Bind() const
    { 
        glUseProgram(m_ShaderID); 
    }

    void OpenGLShader::Unbind() const
    { 
        glUseProgram(0); 
    }


    // utility uniform setter functions
    // ------------------------------------------------------------------------
    void OpenGLShader::SetBool(const std::string& name, bool value) const
    {
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            RD_CORE_ERROR("glShader: SetBool: uniform {0} not found", name);
            return;
        }
        glUniform1i(location, value);
    }
    // ------------------------------------------------------------------------
    void OpenGLShader::SetInt(const std::string& name, int value) const
    {
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            //RD_CORE_ERROR("glShader: SetInt: uniform {0} not found", name);  //for now we have a lot unused texture slots;
            return;
        }
        glUniform1i(location, value);
    }

    void OpenGLShader::SetUInt(const std::string& name, int value) const
    {
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            RD_CORE_WARN("OpenGLShader: SetUInt: uniform {0} not found", name);
            return;
        }  
        glUniform1ui(location, value);
    }

    // ------------------------------------------------------------------------
    void OpenGLShader::SetFloat(const std::string& name, float value) const
    {
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            RD_CORE_ERROR("glShader: SetFloat: uniform {0} not found", name);
            return;
        } 
        glUniform1f(location, value);
    }
    // ------------------------------------------------------------------------
    void OpenGLShader::SetVec2(const std::string& name, const glm::vec2& value) const
    {
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            RD_CORE_ERROR("glShader: SetVec2: uniform {0} not found", name);
            return;
        }
        glUniform2fv(location, 1, &value[0]);
    }

    void OpenGLShader::SetVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(m_ShaderID, name.c_str()), x, y);
    }


    void OpenGLShader::SetIVec2(const std::string& name, const glm::ivec2& value) const
    {
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            RD_CORE_ERROR("glShader: SetVec2: uniform {0} not found", name);
            return;
        }
        glUniform2iv(location, 1, &value[0]);
    }

    void OpenGLShader::SetIVec2(const std::string& name, int x, int y) const
    {
        glUniform2i(glGetUniformLocation(m_ShaderID, name.c_str()), x, y);
    }



    // ------------------------------------------------------------------------
    void OpenGLShader::SetVec3(const std::string& name, const glm::vec3& value) const
    { 
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            RD_CORE_ERROR("glShader: SetVec3: uniform {0} not found", name);
            return;
        }
        glUniform3fv(location, 1, &value[0]);
    }

    void OpenGLShader::SetVec3(const std::string& name, float x, float y, float z) const
    {
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            RD_CORE_ERROR("glShader: SetVec3: uniform {0} not found", name);
            return;
        }
        glUniform3f(location, x, y, z);
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
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            RD_CORE_ERROR("glShader: SetMat3: uniform {0} not found", name);
            return;
        }
        glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& mat) const
    {  
        GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
        if (location == -1) {
            //RD_CORE_ERROR("glShader: SetMat4: uniform {0} not found", name);
            return;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)); 

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
                //shader compilation error
                //glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                //std::cout << "Shader name: " << m_Name << std::endl;
                //std::cout << "APP GET ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                
            }
        }
        else
        {
            //program linking error, usually returns shader compilation error as well
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "Shader name: " << m_Name << " ;directory:" << m_FilePath << std::endl;
                std::cout << "APP GET ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }


}