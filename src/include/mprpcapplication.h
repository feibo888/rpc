#pragma once
#include "mprpcconfig.h"

class MpRpcApplication
{

public:
    static void Init(int argc, char** argv);
    static MpRpcApplication& GetInstance();
    static MpRpcConfig& GetConfig();

private:
    static MpRpcConfig m_config;
    MpRpcApplication(){}
    MpRpcApplication(const MpRpcApplication&) = delete;
    MpRpcApplication(MpRpcApplication&&) = delete;
};
