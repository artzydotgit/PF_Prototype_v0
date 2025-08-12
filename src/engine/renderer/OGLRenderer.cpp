#include "OGLRenderer.h"

OGLRenderer::OGLRenderer() : m_window(nullptr), m_width(1280), m_height(720), m_model(nullptr),
                             m_cameraPos(0.00740962f, 4.58721f, 11.6332f), m_cameraTarget(-0.0218227f, -0.50842f, 0.00721029f), 
                             m_cameraUp(0.0f, 1.0f, 0.0f), m_cameraSpeed(2.5f), m_lastFrame(0.0f) {
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
        std::cerr << "Failed to create GLFW window\n";
        std::cerr << "Error: " << glfwGetError(nullptr) << std::endl;
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
    
    glEnable(GL_DEPTH_TEST);
    
    m_modelRenderer = std::make_unique<ModelRenderer>();
    if (!m_modelRenderer->Initialize()) {
        std::cerr << "Failed to initialize ModelRenderer\n";
        return false;
    }
    
    m_modelRenderer->SetCamera(m_cameraPos, m_cameraTarget, m_cameraUp);
    m_modelRenderer->SetProjection(45.0f, static_cast<float>(m_width)/m_height, 0.1f, 100.0f);
    m_modelRenderer->SetLight(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    
    return true;
}

void OGLRenderer::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void OGLRenderer::ProcessInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }
    
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;
    
    float cameraSpeed = m_cameraSpeed * deltaTime;
    
    if (glfwGetKey(m_window, GLFW_KEY_I) == GLFW_PRESS) {
        m_cameraPos += cameraSpeed * glm::normalize(m_cameraTarget - m_cameraPos);
    }
    if (glfwGetKey(m_window, GLFW_KEY_K) == GLFW_PRESS) {
        m_cameraPos -= cameraSpeed * glm::normalize(m_cameraTarget - m_cameraPos);
    }
    
    if (glfwGetKey(m_window, GLFW_KEY_J) == GLFW_PRESS) {
        glm::vec3 right = glm::normalize(glm::cross(m_cameraTarget - m_cameraPos, m_cameraUp));
        m_cameraPos -= right * cameraSpeed;
        m_cameraTarget -= right * cameraSpeed;
    }
    if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS) {
        glm::vec3 right = glm::normalize(glm::cross(m_cameraTarget - m_cameraPos, m_cameraUp));
        m_cameraPos += right * cameraSpeed;
        m_cameraTarget += right * cameraSpeed;
    }
    
    if (glfwGetKey(m_window, GLFW_KEY_U) == GLFW_PRESS) {
        m_cameraPos += m_cameraUp * cameraSpeed;
        m_cameraTarget += m_cameraUp * cameraSpeed;
    }
    if (glfwGetKey(m_window, GLFW_KEY_O) == GLFW_PRESS) {
        m_cameraPos -= m_cameraUp * cameraSpeed;
        m_cameraTarget -= m_cameraUp * cameraSpeed;
    }
    
    if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS) {
        glm::vec3 direction = m_cameraPos - m_cameraTarget;
        float distance = glm::length(direction);
        
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), cameraSpeed * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 newDirection = glm::vec3(rotation * glm::vec4(direction, 1.0f));
        
        m_cameraPos = m_cameraTarget + glm::normalize(newDirection) * distance;
    }
    
    if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS) {
        glm::vec3 direction = m_cameraPos - m_cameraTarget;
        float distance = glm::length(direction);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), -cameraSpeed * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 newDirection = glm::vec3(rotation * glm::vec4(direction, 1.0f));
        m_cameraPos = m_cameraTarget + glm::normalize(newDirection) * distance;
    }
    
    if (glfwGetKey(m_window, GLFW_KEY_1) == GLFW_PRESS) {
        glm::vec3 direction = m_cameraPos - m_cameraTarget;
        float distance = glm::length(direction);        
        glm::vec3 right = glm::normalize(glm::cross(direction, m_cameraUp));
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), cameraSpeed * 0.5f, right);
        glm::vec3 newDirection = glm::vec3(rotation * glm::vec4(direction, 1.0f));
        m_cameraPos = m_cameraTarget + glm::normalize(newDirection) * distance;
    }
    
    if (glfwGetKey(m_window, GLFW_KEY_2) == GLFW_PRESS) {
        glm::vec3 direction = m_cameraPos - m_cameraTarget;
        float distance = glm::length(direction);
        glm::vec3 right = glm::normalize(glm::cross(direction, m_cameraUp));
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), -cameraSpeed * 0.5f, right);
        glm::vec3 newDirection = glm::vec3(rotation * glm::vec4(direction, 1.0f));
        m_cameraPos = m_cameraTarget + glm::normalize(newDirection) * distance;
    }
    
    static bool mPressed = false;
    if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS) {
        if (!mPressed) {
            std::cout << "Camera Position: (" << m_cameraPos.x << ", " << m_cameraPos.y << ", " << m_cameraPos.z << ")" << std::endl;
            std::cout << "Camera Target: (" << m_cameraTarget.x << ", " << m_cameraTarget.y << ", " << m_cameraTarget.z << ")" << std::endl;
            mPressed = true;
        }
    } else {
        mPressed = false;
    }
    
    UpdateCamera();
}

void OGLRenderer::Render() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (m_model && m_modelRenderer && m_modelRenderer->IsInitialized()) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        m_modelRenderer->RenderModel(*m_model, modelMatrix);
    }
    
    glfwSwapBuffers(m_window);
}

void OGLRenderer::Run() {
    m_lastFrame = static_cast<float>(glfwGetTime());
    
    std::cout << "\n=== Camera Controls ===" << std::endl;
    std::cout << "I/K - Move Forward/Backward" << std::endl;
    std::cout << "J/L - Move Left/Right" << std::endl;
    std::cout << "U/O - Move Up/Down" << std::endl;
    std::cout << "Q/E - Rotate Camera Around Model" << std::endl;
    std::cout << "1/2 - Tilt Camera Up/Down" << std::endl;
    std::cout << "M - Show Camera Position" << std::endl;
    std::cout << "ESC - Exit" << std::endl;
    std::cout << "=====================" << std::endl;
    
    while (!glfwWindowShouldClose(m_window)) {
        ProcessInput();
        Render();
        glfwPollEvents();
    }
}

void OGLRenderer::UpdateCamera() {
    if (m_modelRenderer && m_modelRenderer->IsInitialized()) {
        m_modelRenderer->SetCamera(m_cameraPos, m_cameraTarget, m_cameraUp);
    }
}

void OGLRenderer::Shutdown() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}
