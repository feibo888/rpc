#include "logger.h"

Logger &Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

void Logger::SetLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void Logger::Log(std::string msg)
{
    std::cout << msg;  // 同时输出到控制台
    lck_queue.Push(msg);
}

Logger::Logger()
{
    std::thread writeLosTask([&]()
    {
        for(;;)
        {
            time_t timer = time(nullptr);
            struct tm t;
            localtime_r(&timer, &t);  // 线程安全版本

            if(m_today != t.tm_mday)
            {
                m_today = t.tm_mday;
                if(m_file.is_open())
                {
                    m_file.flush();
                    m_file.close();
                }
                std::string filename;
                std::stringstream ss;
                // ss  << "../log/" 
                ss  << std::setfill('0') << std::setw(4) << (t.tm_year + 1900)
                    << '_' << std::setw(2) << (t.tm_mon + 1)
                    << '_' << std::setw(2) << t.tm_mday
                    << ".log";
                filename = ss.str();

                m_file.open(filename, std::ios::app | std::ios::out);
                // if(!m_file.is_open())
                // {
                //     mkdir(filename.c_str(), 0777);
                //     m_file.open(filename, std::ios::app);
                // }
            }

            std::string msg = lck_queue.Pop();
            std::stringstream ss;
            ss << std::setfill('0') << std::setw(4) << (t.tm_year + 1900)
                << '_' << std::setw(2) << (t.tm_mon + 1)
                << '_' << std::setw(2) << t.tm_mday
                << " " << std::setw(2) << t.tm_hour
                << ":" << std::setw(2) << t.tm_min
                << ":" << std::setw(2) << t.tm_sec
                << " " << msg;
            m_file << ss.str();
            m_file.flush();
            
            
        }
    });

    writeLosTask.detach();
}
