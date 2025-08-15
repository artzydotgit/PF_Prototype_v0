#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include "Shader.h"

class BackgroundRenderer {
public:
    BackgroundRenderer();
    ~BackgroundRenderer();

    bool Initialize();
    void Shutdown();
    void Render();
    
    bool LoadShader(const std::string& vertexPath, const std::string& fragmentPath);
    bool LoadShader(const std::string& shaderName);    
    void SetColor(const glm::vec4& color);
    void SetGradient(const glm::vec4& topColor, const glm::vec4& bottomColor);
    void SetTime(float time);
    bool IsInitialized() const { return m_initialized; }
    bool HasShader() const { return m_shader && m_shader->IsValid(); }

private:
    bool CreateBackgroundMesh();
    void DeleteBackgroundMesh();
    void RenderWithShader();
    void RenderWithoutShader();

private:
    bool m_initialized;
    
    std::unique_ptr<Shader> m_shader;
    
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    
    glm::vec4 m_solidColor;
    glm::vec4 m_topGradientColor;
    glm::vec4 m_bottomGradientColor;
    bool m_useGradient;
    float m_time;
    
    static const std::string DEFAULT_VERTEX_SHADER;
    static const std::string DEFAULT_FRAGMENT_SHADER;
};
