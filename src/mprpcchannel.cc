#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include <sys/types.h>         
#include <sys/socket.h>
#include <arpa/inet.h>
#include "mprpcapplication.h"
#include "zookeeperutil.h"

MpRpcChannel::~MpRpcChannel()
{
}

// 前4个字节表示header_size,然后是rpc_header的内容，然后是rpc调用的参数
// send_str = header_size(4字节) + rpc_header(service_name + method_name + args_size) + args_str
void MpRpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method, 
                                google::protobuf::RpcController *controller, 
                                const google::protobuf::Message *request, 
                                google::protobuf::Message *response, 
                                google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *service = method->service();
    //获取服务名
    std::string service_name = service->name();

    //获取方法名
    std::string method_name = method->name();

    //序列化请求参数，得到args_str
    std::string args_str;
    int arg_size = 0;
    if(request->SerializeToString(&args_str))
    {
        arg_size = args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error");
        return;
    }

    //设置rpc_header
    mprpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(arg_size);

    //序列化rpc_header，得到rpc_header_str
    uint32_t header_size = 0;
    std::string rpc_header_str;
    if(rpc_header.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("serialize rpc_header error");
        return;
    }

    //组装rpc请求的完整字符流
    std::string send_str;
    send_str.insert(0, (char*)&header_size, 4);
    send_str += rpc_header_str;
    send_str += args_str;

    //使用简单tcp传输
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd == -1)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! error:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }
    
    // std::string ip =  MpRpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port = std::stoi(MpRpcApplication::GetInstance().GetConfig().Load("rpcserverport"));

    ZkClient zkclient;
    zkclient.start();

    std::string method_path = "/" + service_name + "/" + method_name;
    std::string host_data = zkclient.GetData(method_path.c_str());
    if(host_data.empty())
    {
        controller->SetFailed(method_path + " not found");
        return;
    }
    int idx = host_data.find(":");
    if(idx == -1)
    {
        controller->SetFailed(method_path + " address error");
        return;
    }

    std::string ip = host_data.substr(0, idx);
    uint16_t port = std::stoi(host_data.substr(idx + 1));


    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip.c_str());

    if(connect(clientfd, (struct sockaddr*)&address, sizeof(address)) == -1)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error error! error:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    if(send(clientfd, send_str.c_str(), send_str.size(), 0) == -1)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "send error error! error:%d", errno);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }

    //接收rpc调用的响应
    char buf[1024] = {0};
    int recv_size = 0;
    if((recv_size = recv(clientfd, buf, 1024, 0)) == -1)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error error! error:%d", errno);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }

    std::string response_str(buf, recv_size);
    if(response->ParseFromString(response_str))
    {
        std::cout << "rpc response: " << response_str << std::endl;
        close(clientfd);
        return;
    }
    else
    {
        controller->SetFailed("parse response error");
    }

    close(clientfd);
}
