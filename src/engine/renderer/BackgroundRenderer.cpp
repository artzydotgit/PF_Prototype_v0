#include "BackgroundRenderer.h"
#include "Shader.h"
#include <iostream>

const std::string BackgroundRenderer::DEFAULT_VERTEX_SHADER = "assets/shaders/background/default.vert";
const std::string BackgroundRenderer::DEFAULT_FRAGMENT_SHADER = "assets/shaders/background/default.frag";

BackgroundRenderer::BackgroundRenderer() 
    : m_initialized(false)
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_solidColor(0.1f, 0.1f, 0.2f, 1.0f)
    , m_topGradientColor(0.1f, 0.1f, 0.2f, 1.0f)
    , m_bottomGradientColor(0.2f, 0.2f, 0.4f, 1.0f)
    , m_useGradient(false)
    , m_time(0.0f) {
}

BackgroundRenderer::~BackgroundRenderer() {
    Shutdown();
}

bool BackgroundRenderer::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    if (!CreateBackgroundMesh()) {
        std::cerr << "Failed to create background mesh" << std::endl;
        return false;
    }
    
    m_initialized = true;
    return true;
}

void BackgroundRenderer::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    DeleteBackgroundMesh();
    m_shader.reset();
    m_initialized = false;
}

void BackgroundRenderer::Render() {
    if (!m_initialized) {
        return;
    }
    
    if (HasShader()) {
        RenderWithShader();
    } else {
        RenderWithoutShader();
    }
}

bool BackgroundRenderer::LoadShader(const std::string& vertexPath, const std::string& fragmentPath) {
    m_shader = std::make_unique<Shader>();
    if (!m_shader->CreateFromFiles(vertexPath, fragmentPath)) {
        std::cerr << "Warning: Failed to load background shader from files. Using fallback rendering." << std::endl;
        m_shader.reset();
        return false;
    }
    return true;
}

bool BackgroundRenderer::LoadShader(const std::string& shaderName) {
    std::string vertexPath = "assets/shaders/background/" + shaderName + ".vert";
    std::string fragmentPath = "assets/shaders/background/" + shaderName + ".frag";
    return LoadShader(vertexPath, fragmentPath);
}

void BackgroundRenderer::SetColor(const glm::vec4& color) {
    m_solidColor = color;
    m_useGradient = false;
}

void BackgroundRenderer::SetGradient(const glm::vec4& topColor, const glm::vec4& bottomColor) {
    m_topGradientColor = topColor;
    m_bottomGradientColor = bottomColor;
    m_useGradient = true;
}

void BackgroundRenderer::SetTime(float time) {
    m_time = time;
}

bool BackgroundRenderer::CreateBackgroundMesh() {
    float vertices[] = {
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    glBindVertexArray(0);
    
    return true;
}

void BackgroundRenderer::DeleteBackgroundMesh() {
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    
    if (m_EBO != 0) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
}

void BackgroundRenderer::RenderWithShader() {
    if (!m_shader || !m_shader->IsValid()) {
        RenderWithoutShader();
        return;
    }
    
    glDisable(GL_DEPTH_TEST);
    m_shader->Use();
    
    if (m_useGradient) {
        m_shader->SetVec4("topColor", m_topGradientColor);
        m_shader->SetVec4("bottomColor", m_bottomGradientColor);
    } else {
        m_shader->SetVec4("color", m_solidColor);
    }
    m_shader->SetFloat("time", m_time);
    
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);    
    glEnable(GL_DEPTH_TEST);
}

void BackgroundRenderer::RenderWithoutShader() {
    if (m_useGradient) {
        glClearColor(m_topGradientColor.r, m_topGradientColor.g, m_topGradientColor.b, m_topGradientColor.a);
    } else {
        glClearColor(m_solidColor.r, m_solidColor.g, m_solidColor.b, m_solidColor.a);
    }
    glClear(GL_COLOR_BUFFER_BIT);
}