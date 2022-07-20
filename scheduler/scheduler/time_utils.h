#pragma once
#include <croncpp.h>
#include <iostream>

static std::string Time()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);

    return "[" +  cron::utils::to_string(*now) + "]";
}

static std::string TimeTotm(int64_t tick)
{
    using namespace std::chrono_literals;
    auto duration = std::chrono::system_clock::duration(tick);

    auto tp = std::chrono::system_clock::time_point(duration);
    auto time = std::chrono::system_clock::to_time_t(tp);
    tm tm;
    cron::utils::time_to_tm(&time, &tm);
    return cron::utils::to_string(tm);
}
