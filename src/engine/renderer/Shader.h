#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader {
public:
    Shader();
    ~Shader();

    bool Create(const std::string& vertexSource, const std::string& fragmentSource);    
    bool CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void Use();
    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat3(const std::string& name, const glm::mat3& value);
    void SetMat4(const std::string& name, const glm::mat4& value);
    GLuint GetProgramID() const { return m_programID; }
    bool IsValid() const { return m_programID != 0; }

private:
    bool CompileShader(const std::string& source, GLenum type, GLuint& shaderID);
    bool CheckShaderErrors(GLuint shaderID);
    bool CheckProgramErrors(GLuint programID);    
    std::string LoadShaderFile(const std::string& filepath);

private:
    GLuint m_programID;
    bool m_initialized;
}; 