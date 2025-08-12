#include "OGLRenderer.h"

OGLRenderer::OGLRenderer() : m_window(nullptr), m_width(1280), m_height(720) {
}

OGLRenderer::~OGLRenderer() {
    Shutdown();
}

bool OGLRenderer::Initialize(int width, int height, const char* title) {
    m_width = width;
    m_height = height;
    
    if (!SetupWindow(width, height, title)) {
        return false;
    }
    
    if (!SetupOpenGL()) {
        return false;
    }
    
    return true;
}

bool OGLRenderer::SetupWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
    
    return true;
}

bool OGLRenderer::SetupOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return false;
    }

    glViewport(0, 0, m_width, m_height);
    return true;
}

void OGLRenderer::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void OGLRenderer::ProcessInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }
}

void OGLRenderer::Render() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(m_window);
}

void OGLRenderer::Run() {
    while (!glfwWindowShouldClose(m_window)) {
        ProcessInput();
        Render();
        glfwPollEvents();
    }
}

void OGLRenderer::Shutdown() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}
