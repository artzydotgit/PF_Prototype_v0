#pragma once

#include "Renderer.h"
#include "OGLRenderer.h"

#ifdef _WIN32
#include "DX9Renderer.h"
#endif

class RendererInit {
public:
    enum class RendererType {
        OpenGL
#ifdef _WIN32
        , DirectX9
#endif
    };

    RendererInit();
    ~RendererInit();

    bool InitializeRenderer(int width = 1280, int height = 720, const char* title = "PF_Prototype_v0", RendererType type = RendererType::OpenGL);
    void StartRenderer();
    void ShutdownRenderer();
    bool IsRendererRunning() const;
    Renderer* GetRenderer() { return m_renderer; }

private:
    Renderer* m_renderer;
    bool m_isInitialized;
};
