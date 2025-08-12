#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader() : m_programID(0), m_initialized(false) {
}

Shader::~Shader() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
    }
}

bool Shader::Create(const std::string& vertexSource, const std::string& fragmentSource) {
    if (m_initialized) {
        glDeleteProgram(m_programID);
        m_programID = 0;
        m_initialized = false;
    }
    
    GLuint vertexShader;
    if (!CompileShader(vertexSource, GL_VERTEX_SHADER, vertexShader)) {
        return false;
    }
    
    GLuint fragmentShader;
    if (!CompileShader(fragmentSource, GL_FRAGMENT_SHADER, fragmentShader)) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);
    
    if (!CheckProgramErrors(m_programID)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_programID);
        m_programID = 0;
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    m_initialized = true;
    return true;
}

bool Shader::CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource = LoadShaderFile(vertexPath);
    std::string fragmentSource = LoadShaderFile(fragmentPath);
    if (vertexSource.empty() || fragmentSource.empty()) {
        return false;
    }    
    return Create(vertexSource, fragmentSource);
}

void Shader::Use() {
    if (m_initialized) {
        glUseProgram(m_programID);
    }
}

void Shader::SetBool(const std::string& name, bool value) {
    if (m_initialized) {
        glUniform1i(glGetUniformLocation(m_programID, name.c_str()), static_cast<int>(value));
    }
}

void Shader::SetInt(const std::string& name, int value) {
    if (m_initialized) {
        glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
    }
}

void Shader::SetFloat(const std::string& name, float value) {
    if (m_initialized) {
        glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
    }
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
    if (m_initialized) {
        glUniform2fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
    }
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
    if (m_initialized) {
        glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
    }
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
    if (m_initialized) {
        glUniform4fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
    }
}

void Shader::SetMat3(const std::string& name, const glm::mat3& value) {
    if (m_initialized) {
        glUniformMatrix3fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
    if (m_initialized) {
        glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
}

bool Shader::CompileShader(const std::string& source, GLenum type, GLuint& shaderID) {
    shaderID = glCreateShader(type);
    const char* sourcePtr = source.c_str();
    glShaderSource(shaderID, 1, &sourcePtr, nullptr);
    glCompileShader(shaderID);
    
    if (!CheckShaderErrors(shaderID)) {
        glDeleteShader(shaderID);
        return false;
    }
    
    return true;
}

bool Shader::CheckShaderErrors(GLuint shaderID) {
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shaderID, 1024, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        return false;
    }
    
    return true;
}

bool Shader::CheckProgramErrors(GLuint programID) {
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(programID, 1024, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
        return false;
    }
    
    return true;
}

std::string Shader::LoadShaderFile(const std::string& filepath) {
    std::string shaderCode;
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shaderFile.open(filepath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();        
        shaderFile.close();        
        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filepath << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
    return shaderCode;
} 