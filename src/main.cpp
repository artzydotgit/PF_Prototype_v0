#include "engine/renderer/RendererInit.h"
#include "engine/backend/CommandArgs.h"
#include <iostream>
#include <algorithm>

int main(int argc, char* argv[]) {
    CommandArgs args;
    if (!args.ParseArgs(argc, argv)) {
        return -1;
    }
    
    if (!args.IsRendererValid()) {
        std::cerr << "Invalid renderer specified. Available options:" << std::endl;
        std::cerr << "  -renderer=opengl" << std::endl;
#ifdef _WIN32
        std::cerr << "  -renderer=dx9" << std::endl;
#endif
        return -1;
    }
    
    std::string rendererType = args.GetRenderer();
    std::transform(rendererType.begin(), rendererType.end(), rendererType.begin(), ::tolower);
    
    RendererInit::RendererType rendererEnum;
    if (rendererType == "opengl") {
        rendererEnum = RendererInit::RendererType::OpenGL;
    }
#ifdef _WIN32
    else if (rendererType == "dx9") {
        rendererEnum = RendererInit::RendererType::DirectX9;
    }
#endif
    else {
        rendererEnum = RendererInit::RendererType::OpenGL;
    }
    
    RendererInit rendererSystem;
    if (!rendererSystem.InitializeRenderer(1280, 720, "PF_Prototype_v0", rendererEnum)) {
        std::cerr << "Failed to initialize renderer system\n";
        return -1;
    }
    
    rendererSystem.StartRenderer();
    return 0;
}