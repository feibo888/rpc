#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>

template<class T>
class LockQueue
{
public:
    T Pop();
    void Push(const T& data);

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition_variable;
};

template <class T>
inline T LockQueue<T>::Pop()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_queue.empty())
    {
        m_condition_variable.wait(lock);
    }
    T data = m_queue.front();
    m_queue.pop();
    return std::move(data);
}

template <class T>
inline void LockQueue<T>::Push(const T &data)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(data);
    m_condition_variable.notify_one();
}
