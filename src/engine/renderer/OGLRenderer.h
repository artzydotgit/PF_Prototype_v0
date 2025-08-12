#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class OGLRenderer {
public:
    OGLRenderer();
    ~OGLRenderer();

    bool Initialize(int width = 1280, int height = 720, const char* title = "PF_Prototype_v0");    
    void Run();    
    void Shutdown();

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
