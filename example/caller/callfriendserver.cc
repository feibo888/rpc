#include "mprpcapplication.h"
#include "friend.pb.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
#include "logger.h"


int main(int argc, char** argv)
{
    //初始化rpc框架
    // LOG_INFO("start init rpc application");
    MpRpcApplication::Init(argc, argv);
    // LOG_INFO("init rpc application success");
    //LOG_INFO("get friend list response");
    //初始化controller
    MpRpcController controller;

    fixbug::GetFriendListsServiceRpc_Stub stub(new MpRpcChannel());


    fixbug::GetFriendListsRequest request;
    request.set_id(1);
    fixbug::GetFriendListsResponse response;

    stub.GetFriendLists(&controller, &request, &response, nullptr);
    

    if(controller.Failed())
    {
        std::cout << "rpc error: " << controller.ErrorText() << std::endl;
        return 0;
    }
    else
    {
        if(response.result().errorcode() == 0)
        {
            for(int i = 0; i < response.name_size(); ++i)
            {
                //LOG_INFO("friend: %s", response.name(i).c_str());
                std::cout << "friend: " << response.name(i) << std::endl;
            }
        }
        else
        {
            std::cout << "get friend list error: " << response.result().errormsg() << std::endl;
        }
    }

    
    
}