#pragma once
#include <string>
#include "lockqueue.h"
#include <fstream>
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <assert.h>

enum LogLevel
{
    INFO,   //普通信息
    DEBUG,
    WARN,
    ERROR   //错误信息
};

class Logger
{

public:
    static Logger& GetInstance();
    void SetLogLevel(LogLevel level);
    void Log(std::string msg);
private:
    int m_logLevel;
    std::ofstream m_file;
    int m_today;
    LockQueue<std::string> lck_queue;
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;

};

#define LOG_BASE(level, logmsgformat, ...)                       \
    do {                                                         \
        Logger &logger = Logger::GetInstance();                  \
        char buf[1024] = {0};                                    \
        snprintf(buf, sizeof(buf), logmsgformat, ##__VA_ARGS__); \
        std::stringstream ss;                                    \
        ss << level << " " << buf << std::endl;                  \
        logger.Log(ss.str());                                    \
    } while (0);


#define LOG_INFO(logmsgformat, ...) do{LOG_BASE("[INFO ]", logmsgformat, ##__VA_ARGS__)}while(0)
#define LOG_DEBUG(logmsgformat, ...) do{LOG_BASE("[DEBUG]", logmsgformat, ##__VA_ARGS__)}while(0)
#define LOG_WARN(logmsgformat, ...) do{LOG_BASE("[WARN ]", logmsgformat, ##__VA_ARGS__)}while(0)
#define LOG_ERROR(logmsgformat, ...) do{LOG_BASE("[ERROR]", logmsgformat, ##__VA_ARGS__)}while(0)
