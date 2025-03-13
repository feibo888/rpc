#pragma once
#include <unordered_map>
#include <string>


class MpRpcConfig
{
public:
    void LoadConfigFile(const char* config_file);
    std::string Load(const char* key);
    void Trim(std::string& line);

private:
    std::unordered_map<std::string, std::string> m_configMap;
};
