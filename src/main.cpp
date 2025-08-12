#include "engine/renderer/RendererInit.h"
#include "engine/backend/CommandArgs.h"
#include "engine/backend/OBJLoader.h"
#include "engine/renderer/ModelRenderer.h"
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
    
    OBJLoader loader;
    
    Model mapModel;
    std::string objPath = "assets/maps/default/map.obj";
    
    std::cout << "Attempting to load: " << objPath << std::endl;
    
    if (loader.LoadModel(objPath, mapModel)) {
        std::cout << "SUCCESS: map.obj loaded successfully!" << std::endl;
        std::cout << "  - Model name: " << mapModel.name << std::endl;
    } else {
        std::cout << "FAILED: Could not load map.obj" << std::endl;
        std::cout << "  - Error: " << loader.GetLastError() << std::endl;
        std::cout << "  - Check if the file exists at: " << objPath << std::endl;
    }
    
    std::cout << std::endl;
    
    RendererInit rendererSystem;
    if (!rendererSystem.InitializeRenderer(1280, 720, "PF_Prototype_v0", rendererEnum)) {
        std::cerr << "Failed to initialize renderer system\n";
        return -1;
    }
    rendererSystem.SetModel(&mapModel);
    rendererSystem.StartRenderer();
    return 0;
}