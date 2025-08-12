#include "engine/renderer/RendererInit.h"
#include <iostream>

int main() {
    RendererInit rendererSystem;
    
    if (!rendererSystem.InitializeRenderer(1280, 720, "PF_Prototype_v0")) {
        std::cerr << "Failed to initialize renderer system\n";
        return -1;
    }
    
    rendererSystem.StartRenderer();
    return 0;
}