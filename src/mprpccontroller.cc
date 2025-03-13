#include "mprpccontroller.h"

MpRpcController::MpRpcController() : m_failed(false), m_errText("")
{
}

MpRpcController::~MpRpcController()
{
}

void MpRpcController::Reset()
{
    m_failed = false;
    m_errText.clear();
}

bool MpRpcController::Failed() const
{
    return m_failed;
}

std::string MpRpcController::ErrorText() const
{
    return m_errText;
}

void MpRpcController::StartCancel()
{
}

void MpRpcController::SetFailed(const std::string &reason)
{
    m_failed = true;
    m_errText = reason;
}

bool MpRpcController::IsCanceled() const
{
    return false;
}

void MpRpcController::NotifyOnCancel(google::protobuf::Closure* callback)
{
}
