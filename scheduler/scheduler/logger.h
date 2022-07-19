#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <sstream>
#include "time_utils.h"

class Logger
{
public:
    static Logger& Instance()
    {
        static Logger instance;
        return instance;
    }

    ~Logger()
    {
        if (_stream.is_open()) {
            _stream.flush();
        }
        _stream.close();
    }
    
    void InitLogger(const std::string& logFilepath)
    {
        _filePath = logFilepath;
        if (_stream.is_open()) {
            _stream.close();
        }
        _stream.open(_filePath, std::ios::app | std::ios::binary);
    }
    
    std::ostream& Stream()
    {
        std::lock_guard<std::recursive_mutex> lck(_lock);
        //_oss << "[" << __TIMESTAMP__ << "]";
        _stream << Time() << _oss.str();
        _stream.flush();
        std::cout << Time() << _oss.str();
        std::cout.flush();
        _oss.str("");
        _oss.clear();
        return _oss;
    }

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator= (const Logger&) = delete;

private:
    std::string   _filePath;
    std::recursive_mutex    _lock;
    std::ostringstream _oss;
    std::ofstream _stream;
};

#define LOG Logger::Instance().Stream()
