#include "mprpcapplication.h"
#include <iostream>
#include <string>
#include "unistd.h"

MpRpcConfig MpRpcApplication::m_config;

void ShowCommandFormat()
{
    std::cout << "format: command -i <config_file>" << std::endl;
}

void MpRpcApplication::Init(int argc, char **argv)
{
    if(argc < 2)
    {
        ShowCommandFormat();
        exit(EXIT_FAILURE);
    }

    int o = 0;
    std::string config_file;
    while((o = getopt(argc, argv, "i:")) != -1)
    {
        switch(o)
        {
            case 'i':
                config_file = optarg;
                break;
            case '?':
                ShowCommandFormat();
                exit(EXIT_FAILURE);
            case ':':
                ShowCommandFormat();
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }
    m_config.LoadConfigFile(config_file.c_str());
}


MpRpcApplication& MpRpcApplication::GetInstance()
{
    static MpRpcApplication app;
    return app;
}

MpRpcConfig& MpRpcApplication::GetConfig()
{
    return m_config;
}
