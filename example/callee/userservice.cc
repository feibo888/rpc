#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "mprpcprovider.h"

class UserService : public fixbug::UserServiceRpc
{

public:
    bool Login(std::string name, std::string password)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << std::endl;
        std::cout << "password: " << password << std::endl;
        return true;
    }

    bool Register(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "name: " << name << std::endl;
        std::cout << "password: " << pwd << std::endl;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
        const ::fixbug::LoginRequest* request,
        ::fixbug::LoginResponse* response,
        ::google::protobuf::Closure* done)
        {
            //从请求体中拿数据，进行业务处理
            std::string name = request->name();
            std::string pwd = request->password();

            //调用业务逻辑
            bool result = Login(name, pwd);

            //将结果写入响应体
            fixbug::ResultCode* code = response->mutable_result();
            code->set_errorcode(0);
            code->set_errormsg("");
            response->set_success(result);

            //执行回调函数
            done->Run();
        }

    void Register(::google::protobuf::RpcController* controller,
        const ::fixbug::RegisterRequest* request,
        ::fixbug::RegisterResponse* response,
        ::google::protobuf::Closure* done)
        {
            //从请求体中拿数据，进行业务处理
            std::string name = request->name();
            std::string pwd = request->password();

            //调用业务逻辑
            bool result = Register(name, pwd);

            //将结果写入响应体
            fixbug::ResultCode* code = response->mutable_result();
            code->set_errorcode(0);
            code->set_errormsg("");
            response->set_success(result);

            //执行回调函数
            done->Run();
        }

};


int main(int argc, char** argv)
{
    //初始化rpc框架
    MpRpcApplication::Init(argc, argv);

    //注册服务,把UserService对象发布到rpc节点上
    MpRpcProvider provider;
    provider.NotifyService(new UserService());

    provider.run();

    return 0;
}

