#pragma once
#include <google/protobuf/service.h>

class MpRpcController : public google::protobuf::RpcController
{

public:
    MpRpcController();
    ~MpRpcController();
    void Reset();


    bool Failed() const;


    std::string ErrorText() const ;

    void StartCancel();


    void SetFailed(const std::string& reason);

    bool IsCanceled() const;

    void NotifyOnCancel(google::protobuf::Closure* callback);
private:
    bool m_failed;
    std::string m_errText;


};
