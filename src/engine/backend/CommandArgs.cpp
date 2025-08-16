#include "CommandArgs.h"
#include <iostream>
#include <algorithm>

CommandArgs::CommandArgs() {
}

bool CommandArgs::ParseArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (!ParseArg(arg)) {
            std::cerr << "Invalid argument format: " << arg << std::endl;
            std::cerr << "Expected format: -key=value" << std::endl;
            return false;
        }
    }
    
    return true;
}

bool CommandArgs::ParseArg(const std::string& arg) {
    if (!IsValidFormat(arg)) {
        return false;
    }
    
    std::string keyValue = arg.substr(1);    
    size_t equalsPos = keyValue.find('=');
    if (equalsPos == std::string::npos) {
        return false;
    }
    std::string key = keyValue.substr(0, equalsPos);
    std::string value = keyValue.substr(equalsPos + 1);
    m_args[key] = value;
    return true;
}

bool CommandArgs::IsValidFormat(const std::string& arg) const {
    if (arg.empty() || arg[0] != '-') {
        return false;
    }
    
    if (arg.find('=') == std::string::npos) {
        return false;
    }
    
    size_t equalsPos = arg.find('=');
    if (equalsPos <= 1 || equalsPos == arg.length() - 1) {
        return false;
    }
    
    return true;
}

std::string CommandArgs::GetRenderer() const {
    auto it = m_args.find("renderer");
    if (it != m_args.end()) {
        return it->second;
    }
    return "opengl";
}

bool CommandArgs::IsRendererValid() const {
    std::string renderer = GetRenderer();
    std::transform(renderer.begin(), renderer.end(), renderer.begin(), ::tolower);
    if (renderer == "opengl") {
        return true;
    }
    return false;
}
