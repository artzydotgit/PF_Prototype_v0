#pragma once

#include "Renderer.h"
#include "ModelRenderer.h"
#include "../Engine.h"
#include "BackgroundRenderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

class OGLRenderer : public Renderer {
public:
    OGLRenderer();
    ~OGLRenderer() override;

    bool Initialize(int width = 1280, int height = 720, const char* title = "PF_Prototype_v0") override;    
    void Run() override;    
    void Shutdown() override;
    
    bool IsInitialized() const override { return m_window != nullptr; }
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    
    void SetModel(const Model* model) { m_model = model; }
    
    void SetBackgroundColor(const glm::vec4& color);
    void SetBackgroundGradient(const glm::vec4& topColor, const glm::vec4& bottomColor);
    bool LoadBackgroundShader(const std::string& shaderName);
    
    void SetEngine(class Engine* engine) { m_engine = engine; }
    void SetVSync(bool enabled);

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    const Model* m_model;
    std::unique_ptr<ModelRenderer> m_modelRenderer;
    std::unique_ptr<BackgroundRenderer> m_backgroundRenderer;
    Engine* m_engine;
    
    glm::vec3 m_cameraPos;
    glm::vec3 m_cameraTarget;
    glm::vec3 m_cameraUp;
    float m_cameraSpeed;
    float m_lastFrame;
    
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    void Render();
    void ProcessInput();
    bool SetupWindow(int width, int height, const char* title);
    bool SetupOpenGL();
    void UpdateCamera();
};
