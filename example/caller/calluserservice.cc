#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"

int main(int argc, char** argv)
{
    //初始化rpc框架
    MpRpcApplication::Init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new MpRpcChannel());

    fixbug::LoginRequest request;
    request.set_name("fb");
    request.set_password("123456");
    fixbug::LoginResponse response;

    stub.Login(nullptr, &request, &response, nullptr);

    fixbug::RegisterRequest request2;
    request2.set_name("fb");
    request2.set_password("123456");
    fixbug::RegisterResponse response2;
    stub.Register(nullptr, &request2, &response2, nullptr);

    if(response.result().errorcode() == 0)
    {
        std::cout << "login success: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "login error: " << response.result().errormsg() << std::endl;
    }


    
    
}