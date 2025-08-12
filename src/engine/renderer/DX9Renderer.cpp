#include "DX9Renderer.h"
#include <iostream>

DX9Renderer::DX9Renderer() : m_width(1280), m_height(720)
#ifdef _WIN32
, m_d3d(nullptr), m_d3dDevice(nullptr), m_backBuffer(nullptr), m_hwnd(nullptr)
#endif
{
}

DX9Renderer::~DX9Renderer() {
    Shutdown();
}

bool DX9Renderer::Initialize(int width, int height, const char* title) {
    m_width = width;
    m_height = height;
    
#ifdef _WIN32
    if (!SetupWindow(width, height, title)) {
        return false;
    }
    
    if (!SetupDirectX()) {
        return false;
    }
    
    return true;
#else
    std::cerr << "DirectX9 renderer is not available on this platform. Use OpenGL instead.\n";
    return false;
#endif
}

bool DX9Renderer::SetupWindow(int width, int height, const char* title) {
#ifdef _WIN32
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"DX9RendererWindow";
    
    if (!RegisterClassEx(&wc)) {
        std::cerr << "Failed to register window class\n";
        return false;
    }
    
    m_hwnd = CreateWindowEx(
        0,
        L"DX9RendererWindow",
        L"PF_Prototype_v0",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr, nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (!m_hwnd) {
        std::cerr << "Failed to create window\n";
        return false;
    }
    
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    
    return true;
#else
    return false;
#endif
}

bool DX9Renderer::SetupDirectX() {
#ifdef _WIN32
    m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!m_d3d) {
        std::cerr << "Failed to create Direct3D interface\n";
        return false;
    }
    
    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = m_hwnd;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = m_width;
    d3dpp.BackBufferHeight = m_height;
    d3dpp.BackBufferCount = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    
    HRESULT hr = m_d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        m_hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &m_d3dDevice
    );
    
    if (FAILED(hr)) {
        std::cerr << "Failed to create Direct3D device\n";
        return false;
    }
    
    hr = m_d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_backBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to get back buffer\n";
        return false;
    }
    
    return true;
#else
    return false;
#endif
}

void DX9Renderer::ProcessInput() {
#ifdef _WIN32
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#else
#endif
}

void DX9Renderer::Render() {
#ifdef _WIN32
    if (!m_d3dDevice) return;
    
    m_d3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(25, 25, 50), 1.0f, 0);
    
    if (SUCCEEDED(m_d3dDevice->BeginScene())) {        
        m_d3dDevice->EndScene();
    }    
    m_d3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
#else
#endif
}

void DX9Renderer::Run() {
#ifdef _WIN32
    while (true) {
        ProcessInput();
        Render();
        
        if (!IsWindow(m_hwnd)) {
            break;
        }
    }
#else
    std::cerr << "DirectX9 renderer is not available on this platform.\n";
#endif
}

void DX9Renderer::Shutdown() {
#ifdef _WIN32
    if (m_backBuffer) {
        m_backBuffer->Release();
        m_backBuffer = nullptr;
    }
    
    if (m_d3dDevice) {
        m_d3dDevice->Release();
        m_d3dDevice = nullptr;
    }
    
    if (m_d3d) {
        m_d3d->Release();
        m_d3d = nullptr;
    }
    
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
#else
#endif
}

#ifdef _WIN32
LRESULT CALLBACK DX9Renderer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_PAINT:
            ValidateRect(hwnd, nullptr);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#endif 