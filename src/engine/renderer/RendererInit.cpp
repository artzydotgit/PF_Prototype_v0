#include "RendererInit.h"
#include <iostream>

RendererInit::RendererInit() : m_renderer(nullptr), m_isInitialized(false) {
}

RendererInit::~RendererInit() {
    ShutdownRenderer();
}

bool RendererInit::InitializeRenderer(int width, int height, const char* title) {
    if (m_isInitialized) {
        std::cout << "Renderer already initialized\n";
        return true;
    }

    m_renderer = new OGLRenderer();
    
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