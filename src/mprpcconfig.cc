#include "mprpcconfig.h"
#include <fstream>
#include <iostream>

void MpRpcConfig::Trim(std::string &line)
{
    size_t index = line.find_first_not_of(' ');
    if(index != std::string::npos)
    {
        line = line.substr(index);
    }
    index = line.find_last_not_of(' ');
    if(index != std::string::npos)
    {
        line = line.substr(0, line.find_last_not_of(' ') + 1);
    }
}

void MpRpcConfig::LoadConfigFile(const char *config_file)
{
    std::fstream config;
    config.open(config_file, std::ios::in);
    if(!config.is_open())
    {
        std::cout << "config file open error" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while(std::getline(config, line))
    {
        Trim(line);
        if(line[0] == '#' || line.empty())
        {
            continue;
        }
        size_t pos = line.find('=');
        if(pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            Trim(key);
            Trim(value);
            m_configMap.insert({key, value});
        }
    }
    config.close();
}

std::string MpRpcConfig::Load(const char *key)
{
    auto it = m_configMap.find(key);
    if(it == m_configMap.end())
    {
        std::cout << "key not found" << std::endl;
        exit(EXIT_FAILURE);
    }
    return it->second;
}
