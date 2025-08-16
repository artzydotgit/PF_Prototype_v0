#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

class RendererInit;
class OBJLoader;
class Model;

class Engine {
public:
    enum class RendererType {
        OpenGL,
        Auto
    };

    Engine();
    ~Engine();

    bool Initialize(int width = 1280, int height = 720, const char* title = "PF_Prototype_v0", RendererType type = RendererType::Auto);
    void Shutdown();
    void Run();
    bool IsRunning() const;

    bool SetRenderer(RendererType type);
    RendererType GetCurrentRenderer() const;
    
    bool LoadModel(const std::string& filepath, const std::string& modelName = "");
    bool LoadModel(const std::string& filepath, const Model& model);
    void SetActiveModel(const std::string& modelName);
    const Model* GetActiveModel() const;
    
    bool LoadBackgroundShader(const std::string& shaderName);
    void SetBackgroundColor(const glm::vec4& color);
    void SetBackgroundGradient(const glm::vec4& topColor, const glm::vec4& bottomColor);
    void SetBackgroundSolid(const glm::vec4& color);
    
    void SetWindowSize(int width, int height);
    void SetWindowTitle(const std::string& title);
    void SetVSync(bool enabled);
    void SetFPSLimit(int fps);
    
    float GetDeltaTime() const;
    float GetFPS() const;
    void PrintSystemInfo() const;
    void UpdatePerformanceMetrics();

private:
    std::unique_ptr<RendererInit> m_rendererSystem;
    std::unique_ptr<OBJLoader> m_modelLoader;
    
    bool m_initialized;
    bool m_running;
    RendererType m_currentRenderer;
    
    int m_width;
    int m_height;
    std::string m_title;
    
    float m_deltaTime;
    float m_fps;
    float m_lastFrameTime;
    float m_targetFrameTime;
    
    std::string m_activeModelName;
    std::vector<std::pair<std::string, Model>> m_loadedModels;
    
    bool InitializeRenderer();
    void LoadDefaultAssets();
};
