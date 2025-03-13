#pragma once
#include <string>
#include <zookeeper/zookeeper.h>
#include "mprpcapplication.h"
#include <iostream>
#include <semaphore.h>

class ZkClient
{

public:
    ZkClient();
    ~ZkClient();
    void start();
    void Create(const char* path, const char* data, int datalen, int state = 0);
    std::string GetData(const char* path);

private:
    zhandle_t *m_zhandle;

};

