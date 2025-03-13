#pragma once
#include <google/protobuf/service.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <google/protobuf/descriptor.h>
#include <string>
#include <functional>
#include <unordered_map>


class MpRpcProvider
{
public:
    void NotifyService(google::protobuf::Service* service);
    void run();
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp timestamp);
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response);

private:
    muduo::net::EventLoop m_eventLoop;

    struct ServiceInfo
    {
        google::protobuf::Service* m_service;   //服务端的对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap; //服务端对象的方法名和具体的方法描述
    };
    
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;  //保存服务端对象的所有信息
};

