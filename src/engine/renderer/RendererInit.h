#pragma once

#include "OGLRenderer.h"

class RendererInit {
public:
    RendererInit();
    ~RendererInit();

    bool InitializeRenderer(int width = 1280, int height = 720, const char* title = "PF_Prototype_v0");    
    void StartRenderer();
    void ShutdownRenderer();
    bool IsRendererRunning() const;
    OGLRenderer* GetRenderer() { return m_renderer; }

private:
    OGLRenderer* m_renderer;
    bool m_isInitialized;
};
