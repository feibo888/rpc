#include "mprpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"


void MpRpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;

    //获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDec = service->GetDescriptor();
    //获取服务对象的名字
    std::string service_name = pserviceDec->name();
    //获取服务对象的方法的数量
    int method_count = pserviceDec->method_count();

    for(int i = 0; i < method_count; ++i)
    {
        //获取服务对象指定下标的服务方法描述
        const google::protobuf::MethodDescriptor* pMethodDec = pserviceDec->method(i);
        std::string method_name = pMethodDec->name();
        service_info.m_methodMap.insert({method_name, pMethodDec});
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}

void MpRpcProvider::run()
{
    std::string ip =  MpRpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = std::stoi(MpRpcApplication::GetInstance().GetConfig().Load("rpcserverport"));
    muduo::net::InetAddress address(ip, port);

    muduo::net::TcpServer server(&m_eventLoop, address, "MpRpcProvider");

    //绑定连接回调和消息读写回调方法，分离网络代码和业务代码
    server.setConnectionCallback(std::bind(&MpRpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&MpRpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(8);


    //把要发布的服务注册到zookeerper上
    ZkClient zkclient;
    zkclient.start();

    //服务名注册 service_name
    for(auto& sp : m_serviceMap)
    {
        std::string service_path = "/" + sp.first;
        zkclient.Create(service_path.c_str(), nullptr, 0, 0);
        for(auto& mp : sp.second.m_methodMap)
        {   
            //方法名注册 method_name
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            //method为临时性结点
            zkclient.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }


    std::cout << "rpc server is running... ip: " << ip << " port: " << port << std::endl; 

    server.start();
    m_eventLoop.loop();

    
}

void MpRpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if(!conn->connected())
    {
        conn->shutdown();
    }
}


//为解决TCP粘包问题，需要在OnMessage方法中对接收到的rpc调用请求的字符流进行解析
//header_size(4字节) + rpc_header + args
//header_size: rpc_header的长度
//rpc_header = service_name + method_name + args_size
//service_name: 服务名字
//method_name: 方法名字
//args_size: 参数的长度
//args: 参数的序列化字符流
void MpRpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp timestamp)
{
    //网络上接收的远程rpc调用请求的字符流
    std::string recv_buf = buffer->retrieveAllAsString();

    //从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    //根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpc_header;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if(rpc_header.ParseFromString(rpc_header_str))
    {
        service_name = rpc_header.service_name();
        method_name = rpc_header.method_name();
        args_size = rpc_header.args_size();
    }
    else
    {
        std::cout << "rpc_header_str parse error, rpc_geader_str: " << rpc_header_str << std::endl;
        return;
    }

    std::string args_str = recv_buf.substr(4 + header_size, args_size);   
    
    LOG_INFO("service_name: %s, method_name: %s, args_size: %d", service_name.c_str(), method_name.c_str(), args_size);

    // std::cout << "header_size: " << header_size << std::endl;
    // std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    // std::cout << "service_name: " << service_name << std::endl;
    // std::cout << "method_name: " << method_name << std::endl;
    // std::cout << "args_size: " << args_size << std::endl;
    // std::cout << "args_str: " << args_str << std::endl;

    //获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end())
    {
        //std::cout << "service_name: " << service_name << " not found" << std::endl;
        LOG_ERROR("service_name: %s not found", service_name.c_str());
        return;
    }

    google::protobuf::Service *service = it->second.m_service;
    const google::protobuf::MethodDescriptor* method = it->second.m_methodMap[method_name];

    if(method == nullptr)
    {
        LOG_ERROR("method_name: %s not found", method_name.c_str());
        //std::cout << "method_name: " << method_name << " not found" << std::endl;
        return;
    }

    //生成rpc方法调用的请求request和响应response
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
        LOG_ERROR("request parse error");
        std::cout << "request parse error" << std::endl;
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    //给下面的method方法的调用，绑定一个closure的回调函数
    google::protobuf::Closure* done = google::protobuf::NewCallback
                                        <MpRpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                                        (this, &MpRpcProvider::SendRpcResponse, conn, response);

    //在框架上根据远端rpc请求，调用当前rpc节点上发布的服务方法
    service->CallMethod(method, nullptr, request, response, done);
}

void MpRpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    }
    else
    {
        std::cout << "serialize response_str error" << std::endl;
    }
    conn->shutdown();
}
