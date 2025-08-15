#include "Engine.h"
#include "renderer/RendererInit.h"
#include "backend/OBJLoader.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

class OGLRenderer;

Engine::Engine() 
    : m_initialized(false)
    , m_running(false)
    , m_currentRenderer(RendererType::Auto)
    , m_width(1280)
    , m_height(720)
    , m_title("PF_Prototype_v0")
    , m_deltaTime(0.0f)
    , m_fps(0.0f)
    , m_lastFrameTime(0.0f)
    , m_targetFrameTime(0.0f) {
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize(int width, int height, const char* title, RendererType type) {
    if (m_initialized) {
        std::cout << "Engine already initialized" << std::endl;
        return true;
    }

    m_width = width;
    m_height = height;
    m_title = title ? title : "PF_Prototype_v0";
    
    if (type == RendererType::Auto) {
        m_currentRenderer = RendererType::OpenGL;
    } else {
        m_currentRenderer = type;
    }

    m_modelLoader = std::make_unique<OBJLoader>();
    
    if (!InitializeRenderer()) {
        std::cerr << "Failed to initialize renderer system" << std::endl;
        return false;
    }

    LoadDefaultAssets();

    m_initialized = true;
    std::cout << "Engine initialized successfully" << std::endl;
    return true;
}

void Engine::Shutdown() {
    if (!m_initialized) {
        return;
    }

    if (m_rendererSystem) {
        m_rendererSystem->ShutdownRenderer();
    }

    m_loadedModels.clear();
    m_modelLoader.reset();
    m_rendererSystem.reset();
    
    m_initialized = false;
    m_running = false;
    
    std::cout << "Engine shutdown complete" << std::endl;
}

void Engine::Run() {
    if (!m_initialized || !m_rendererSystem) {
        std::cerr << "Cannot run engine: not initialized" << std::endl;
        return;
    }

    m_running = true;
    std::cout << "Starting engine..." << std::endl;
    
    m_lastFrameTime = static_cast<float>(glfwGetTime());
    
    m_rendererSystem->StartRenderer();
    
    m_running = false;
}

bool Engine::IsRunning() const {
    return m_running && m_initialized;
}

bool Engine::SetRenderer(RendererType type) {
    if (m_initialized) {
        std::cerr << "Cannot change renderer after initialization" << std::endl;
        return false;
    }
    
    m_currentRenderer = type;
    return true;
}

Engine::RendererType Engine::GetCurrentRenderer() const {
    return m_currentRenderer;
}

bool Engine::LoadModel(const std::string& filepath, const std::string& modelName) {
    if (!m_modelLoader) {
        std::cerr << "Model loader not initialized" << std::endl;
        return false;
    }

    std::string name = modelName.empty() ? filepath : modelName;
    
    for (const auto& pair : m_loadedModels) {
        if (pair.first == name) {
            std::cout << "Model '" << name << "' already loaded" << std::endl;
            return true;
        }
    }

    Model model;
    if (m_modelLoader->LoadModel(filepath, model)) {
        m_loadedModels.emplace_back(name, std::move(model));
        
        if (m_loadedModels.size() == 1) {
            m_activeModelName = name;
            if (m_rendererSystem) {
                m_rendererSystem->SetModel(&m_loadedModels.back().second);
            }
        }
        
        std::cout << "SUCCESS: Model '" << name << "' loaded from " << filepath << std::endl;
        return true;
    } else {
        std::cerr << "FAILED: Could not load model from " << filepath << std::endl;
        std::cerr << "Error: " << m_modelLoader->GetLastError() << std::endl;
        return false;
    }
}

bool Engine::LoadModel(const std::string& filepath, const Model& model) {
    return LoadModel(filepath, filepath);
}

void Engine::SetActiveModel(const std::string& modelName) {
    for (const auto& pair : m_loadedModels) {
        if (pair.first == modelName) {
            m_activeModelName = modelName;
            if (m_rendererSystem) {
                m_rendererSystem->SetModel(&pair.second);
            }
            std::cout << "Active model set to: " << modelName << std::endl;
            return;
        }
    }
    
    std::cerr << "Model '" << modelName << "' not found" << std::endl;
}

const Model* Engine::GetActiveModel() const {
    for (const auto& pair : m_loadedModels) {
        if (pair.first == m_activeModelName) {
            return &pair.second;
        }
    }
    return nullptr;
}

bool Engine::LoadBackgroundShader(const std::string& shaderName) {
    if (!m_rendererSystem) {
        std::cerr << "Renderer system not initialized" << std::endl;
        return false;
    }
    
    bool success = m_rendererSystem->LoadBackgroundShader(shaderName);
    if (success) {
        std::cout << "Background shader '" << shaderName << "' loaded successfully" << std::endl;
    } else {
        std::cout << "Failed to load background shader '" << shaderName << "', using fallback" << std::endl;
    }
    return success;
}

void Engine::SetBackgroundColor(const glm::vec4& color) {
    if (m_rendererSystem) {
        m_rendererSystem->SetBackgroundColor(color);
    }
}

void Engine::SetBackgroundGradient(const glm::vec4& topColor, const glm::vec4& bottomColor) {
    if (m_rendererSystem) {
        m_rendererSystem->SetBackgroundGradient(topColor, bottomColor);
    }
}

void Engine::SetBackgroundSolid(const glm::vec4& color) {
    SetBackgroundColor(color);
}

void Engine::SetWindowSize(int width, int height) {
    if (!m_initialized) {
        m_width = width;
        m_height = height;
    }
}

void Engine::SetWindowTitle(const std::string& title) {
    if (!m_initialized) {
        m_title = title;
    }
}

void Engine::SetVSync(bool enabled) {
    if (m_rendererSystem) {
        if (auto* renderer = m_rendererSystem->GetRenderer()) {
            if (auto* oglRenderer = dynamic_cast<OGLRenderer*>(renderer)) {
                oglRenderer->SetVSync(enabled);
            }
        }
    }
}

void Engine::SetFPSLimit(int fps) {
    if (fps > 0) {
        m_targetFrameTime = 1.0f / static_cast<float>(fps);
    } else {
        m_targetFrameTime = 0.0f;
    }
}

float Engine::GetDeltaTime() const {
    return m_deltaTime;
}

float Engine::GetFPS() const {
    return m_fps;
}

void Engine::PrintSystemInfo() const {
    std::cout << "\n=== Engine System Info ===" << std::endl;
    std::cout << "Initialized: " << (m_initialized ? "Yes" : "No") << std::endl;
    std::cout << "Running: " << (m_running ? "Yes" : "No") << std::endl;
    std::cout << "Window: " << m_width << "x" << m_height << std::endl;
    std::cout << "Title: " << m_title << std::endl;
    std::cout << "Renderer: " << (int)m_currentRenderer << std::endl;
    std::cout << "Models loaded: " << m_loadedModels.size() << std::endl;
    std::cout << "Active model: " << (m_activeModelName.empty() ? "None" : m_activeModelName) << std::endl;
    std::cout << "FPS: " << m_fps << std::endl;
    std::cout << "Delta time: " << m_deltaTime << std::endl;
    std::cout << "FPS Limit: " << (m_targetFrameTime > 0.0f ? std::to_string(static_cast<int>(1.0f / m_targetFrameTime)) : "None") << std::endl;
    std::cout << "=========================" << std::endl;
}

bool Engine::InitializeRenderer() {
    m_rendererSystem = std::make_unique<RendererInit>();
    
    RendererInit::RendererType rendererType;
    switch (m_currentRenderer) {
        case RendererType::OpenGL:
            rendererType = RendererInit::RendererType::OpenGL;
            break;
#ifdef _WIN32
        case RendererType::DirectX9:
            rendererType = RendererInit::RendererType::DirectX9;
            break;
#endif
        default:
            rendererType = RendererInit::RendererType::OpenGL;
            break;
    }
    
    if (!m_rendererSystem->InitializeRenderer(m_width, m_height, m_title.c_str(), rendererType)) {
        std::cerr << "Failed to initialize renderer system" << std::endl;
        return false;
    }
    
    if (auto* renderer = m_rendererSystem->GetRenderer()) {
        if (auto* oglRenderer = dynamic_cast<OGLRenderer*>(renderer)) {
            oglRenderer->SetEngine(this);
        }
    }
    
    return true;
}

void Engine::UpdatePerformanceMetrics() {
    float currentTime = static_cast<float>(glfwGetTime());
    m_deltaTime = currentTime - m_lastFrameTime;
    m_lastFrameTime = currentTime;
    
    if (m_deltaTime > 0.0f) {
        m_fps = 1.0f / m_deltaTime;
    }
    
    if (m_targetFrameTime > 0.0f && m_deltaTime < m_targetFrameTime) {
        float sleepTime = m_targetFrameTime - m_deltaTime;
        if (sleepTime > 0.0f) {
            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>(sleepTime * 1000000.0f)));            
            float newTime = static_cast<float>(glfwGetTime());
            m_deltaTime = newTime - (m_lastFrameTime - m_deltaTime);
            m_lastFrameTime = newTime;
            
            if (m_deltaTime > 0.0f) {
                m_fps = 1.0f / m_deltaTime;
            }
        }
    }
}

void Engine::LoadDefaultAssets() {
    std::string defaultMapPath = "assets/maps/default/map.obj";
    if (LoadModel(defaultMapPath, "default_map")) {
        std::cout << "Default map loaded successfully" << std::endl;
    } else {
        std::cout << "Default map not found, continuing without it" << std::endl;
    }
    
    if (LoadBackgroundShader("animated")) {
        std::cout << "Animated background shader loaded" << std::endl;
        SetBackgroundGradient(
            glm::vec4(0.1f, 0.1f, 0.3f, 1.0f),
            glm::vec4(0.3f, 0.1f, 0.1f, 1.0f)
        );
    } else {
        std::cout << "Using fallback background rendering" << std::endl;
        SetBackgroundColor(glm::vec4(0.2f, 0.1f, 0.3f, 1.0f));
    }
    SetFPSLimit(60);
    SetVSync(false);
}