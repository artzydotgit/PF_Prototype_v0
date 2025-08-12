#pragma once

#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;    
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    void Render();
    void ProcessInput();
    bool SetupWindow(int width, int height, const char* title);
    bool SetupOpenGL();
};
