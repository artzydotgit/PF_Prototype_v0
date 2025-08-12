#pragma once

#include "Renderer.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <d3d9.h>
#endif

class DX9Renderer : public Renderer {
public:
    DX9Renderer();
    ~DX9Renderer() override;

    bool Initialize(int width = 1280, int height = 720, const char* title = "PF_Prototype_v0") override;
    void Run() override;
    void Shutdown() override;
    
    bool IsInitialized() const override { 
#ifdef _WIN32
        return m_d3dDevice != nullptr; 
#else
        return false;
#endif
    }
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }

private:
#ifdef _WIN32
    LPDIRECT3D9 m_d3d;
    LPDIRECT3DDEVICE9 m_d3dDevice;
    LPDIRECT3DSURFACE9 m_backBuffer;
    
    HWND m_hwnd;
#endif
    int m_width;
    int m_height;
    
    bool SetupWindow(int width, int height, const char* title);
    bool SetupDirectX();
    void Render();
    void ProcessInput();
    
#ifdef _WIN32
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
}; 