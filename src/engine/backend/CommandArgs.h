#pragma once

#include <string>
#include <map>

class CommandArgs {
public:
    CommandArgs();
    ~CommandArgs() = default;

    bool ParseArgs(int argc, char* argv[]);    
    std::string GetRenderer() const;    
    bool IsRendererValid() const;

private:
    std::map<std::string, std::string> m_args;
    bool ParseArg(const std::string& arg);    
    bool IsValidFormat(const std::string& arg) const;
};
