#pragma once

class Renderer {
public:
    virtual ~Renderer() = default;
    
    virtual bool Initialize(int width, int height, const char* title) = 0;
    virtual void Run() = 0;
    virtual void Shutdown() = 0;    
    virtual bool IsInitialized() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
}; 