#include "RendererInit.h"
#include <iostream>

RendererInit::RendererInit() : m_renderer(nullptr), m_isInitialized(false) {
}

RendererInit::~RendererInit() {
    ShutdownRenderer();
}

bool RendererInit::InitializeRenderer(int width, int height, const char* title, RendererType type) {
    if (m_isInitialized) {
        std::cout << "Renderer already initialized\n";
        return true;
    }

    switch (type) {
        case RendererType::OpenGL:
            m_renderer = new OGLRenderer();
            std::cout << "Creating OpenGL renderer...\n";
            break;
        default:
            std::cerr << "Unknown renderer type\n";
            return false;
    }
    
    if (!m_renderer) {
        std::cerr << "Failed to create renderer instance\n";
        return false;
    }

    if (!m_renderer->Initialize(width, height, title)) {
        std::cerr << "Failed to initialize renderer\n";
        delete m_renderer;
        m_renderer = nullptr;
        return false;
    }

    m_isInitialized = true;
    std::cout << "Renderer initialized successfully\n";
    return true;
}

void RendererInit::StartRenderer() {
    if (!m_isInitialized || !m_renderer) {
        std::cerr << "Cannot start renderer: not initialized\n";
        return;
    }

    std::cout << "Starting renderer...\n";
    m_renderer->Run();
}

void RendererInit::ShutdownRenderer() {
    if (m_renderer) {
        m_renderer->Shutdown();
        delete m_renderer;
        m_renderer = nullptr;
    }
    
    m_isInitialized = false;
    std::cout << "Renderer shutdown complete\n";
}

bool RendererInit::IsRendererRunning() const {
    return m_isInitialized && m_renderer != nullptr;
}

void RendererInit::SetModel(const Model* model) {
    if (m_renderer) {
        if (OGLRenderer* oglRenderer = dynamic_cast<OGLRenderer*>(m_renderer)) {
            oglRenderer->SetModel(model);
        }
    }
}

void RendererInit::SetBackgroundColor(const glm::vec4& color) {
    if (m_renderer) {
        if (OGLRenderer* oglRenderer = dynamic_cast<OGLRenderer*>(m_renderer)) {
            oglRenderer->SetBackgroundColor(color);
        }
    }
}

void RendererInit::SetBackgroundGradient(const glm::vec4& topColor, const glm::vec4& bottomColor) {
    if (m_renderer) {
        if (OGLRenderer* oglRenderer = dynamic_cast<OGLRenderer*>(m_renderer)) {
            oglRenderer->SetBackgroundGradient(topColor, bottomColor);
        }
    }
}

bool RendererInit::LoadBackgroundShader(const std::string& shaderName) {
    if (m_renderer) {
        if (OGLRenderer* oglRenderer = dynamic_cast<OGLRenderer*>(m_renderer)) {
            return oglRenderer->LoadBackgroundShader(shaderName);
        }
    }
    return false;
}