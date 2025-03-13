#include <iostream>
#include <string>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "mprpcprovider.h"
#include <vector>
#include "logger.h"


class myFriendLists : public fixbug::GetFriendListsServiceRpc
{

public:
    std::vector<std::string> GetFriendLists(int id)
    {
        std::cout << "doing local service: GetFriendLists" << std::endl;
        std::cout << "id: " << id << std::endl;
        std::vector<std::string> friends;
        friends.push_back("zhangsan");
        friends.push_back("lisi");
        friends.push_back("wangwu");
        return friends;

    }

    void GetFriendLists(::google::protobuf::RpcController* controller,
                        const ::fixbug::GetFriendListsRequest* request,
                        ::fixbug::GetFriendListsResponse* response,
                        ::google::protobuf::Closure* done)
        {
            int id = request->id();
            std::vector<std::string> friends = GetFriendLists(id);
            

            for(int i = 0; i < friends.size(); ++i)
            {
                response->add_name(friends[i]);
            }
            
            fixbug::ResultCode* resultcode = response->mutable_result();
            resultcode->set_errorcode(0);
            resultcode->set_errormsg("");
            done->Run();

        }

};


int main(int argc, char** argv)
{
    //初始化rpc框架
    MpRpcApplication::Init(argc, argv);
    LOG_INFO("start init rpc application");
    LOG_INFO("init rpc application success");

    //注册服务,把UserService对象发布到rpc节点上
    MpRpcProvider provider;
    provider.NotifyService(new myFriendLists());

    provider.run();

    return 0;
}