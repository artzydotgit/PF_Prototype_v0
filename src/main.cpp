#include "engine/Engine.h"
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
        return -1;
    }
    
    std::string rendererType = args.GetRenderer();
    std::transform(rendererType.begin(), rendererType.end(), rendererType.begin(), ::tolower);
    
    Engine::RendererType rendererEnum;
    if (rendererType == "opengl") {
        rendererEnum = Engine::RendererType::OpenGL;
    }
    else {
        rendererEnum = Engine::RendererType::OpenGL;
    }
    
    std::cout << "Initializing PF_Prototype_v0 Engine..." << std::endl;
    std::cout << "Renderer: " << rendererType << std::endl;
    std::cout << std::endl;
    
    Engine engine;    
    engine.SetRenderer(rendererEnum);    
    if (!engine.Initialize(1280, 720, "PF_Prototype_v0")) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return -1;
    }    
    engine.PrintSystemInfo();    
    engine.Run();
    return 0;
}