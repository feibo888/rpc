#include "zookeeperutil.h"

void global_watch(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx)
{
    if(type == ZOO_SESSION_EVENT)
    {
        if(state == ZOO_CONNECTED_STATE)
        {
            sem_t* sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient() : m_zhandle(nullptr)
{
}

ZkClient::~ZkClient()
{
    if(m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle);
    }
}

void ZkClient::start()
{
    std::string host = MpRpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MpRpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr = host + ":" + port;

    m_zhandle = zookeeper_init(connstr.c_str(), global_watch, 30000, nullptr, nullptr, 0);
    if(m_zhandle == nullptr)
    {
        std::cout << "zookeeper_init error!" << std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    std::cout << "zookeeper_init success!" << std::endl;
}

void ZkClient::Create(const char *path, const char *data, int datalen, int state)
{
    char path_buffer[128] = {0};
    int bufferlen = sizeof(path_buffer);
    int flag;
    flag = zoo_exists(m_zhandle, path, 0, nullptr);

    if(flag == ZNONODE)
    {
        flag = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
        if(flag == ZOK)
        {
            std::cout << "create node success! path: " << path << std::endl;
        }
        else
        {
            std::cout << "flag: " << flag << std::endl;
            std::cout << "create node error! path: " << path << std::endl;
            exit(EXIT_FAILURE);
        }
    }


}

std::string ZkClient::GetData(const char *path)
{
    char buff[64];
    int bufferlen = sizeof(buff);
    int flag = zoo_get(m_zhandle, path, 0, buff, &bufferlen, nullptr);

    if(flag != ZOK)
    {
        std::cout << "get data error! path: " << path << std::endl;
        return "";
    }
    else
    {
        return std::move(buff);
    }
}
