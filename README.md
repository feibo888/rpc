# RPC框架实现

## RPC？

RPC是远程过程调用（Remote Procedure Call）的缩写，可以通过网络从远程服务器上请求服务（调用远端服务器上的函数并获取返回结果）。简单来说，客户端程序可以像调用本地函数一样直接调用运行在服务端的函数。

## **为什么要用RPC？**

简化远程调用的复杂性

在分布式系统中，服务往往部署在不同的机器或网络中。如果没有RPC，开发人员需要手动编写复杂的网络通信代码，比如：

- 创建和管理连接。
- 数据的序列化和反序列化。
- 请求和响应的发送与接收。
- 错误处理（如超时、断线重连等）。

RPC对这些底层逻辑进行了封装，开发人员只需专注于业务逻辑，无需关心底层的通信细节。

## 收获

- 深入理解分布式RPC架构的核心原理，掌握服务注册与发现的完整流程。
- 掌握Zookeeper的应用场景、节点监听机制及配置管理，增强对分布式协调服务的理解。
- 熟练使用Muduo网络库，实现高并发网络通信，提升了对网络编程的理解和实践能力。
- 提升了在C++网络通信、分布式架构和日志系统设计等方面的开发能力。

## Todo
- 实现负载均衡
- 引入连接池