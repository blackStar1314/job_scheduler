#pragma once
#include "job.h"
#include "croncpp.h"

class InJob : public Job
{
public:
    explicit InJob(const std::string& id, const std::string& time, Function&& work, bool repeated = false)
        : Job(id, work, repeated)
        , _triggledTime(Clock::time_point(Clock::duration(0)))
    {
        _valid = FormatTimePoint(time);
    }

    Clock::time_point GetTriggleTime() override 
    {
        return CaculateTriggleTime();
    }

protected:

    bool FormatTimePoint(const std::string time)
    {
        using namespace std::chrono_literals;

        auto tm = cron::utils::to_tm(time);
        _triggledTime = Clock::from_time_t(cron::utils::tm_to_time(tm));

        return IsRepeated() || !IsExpired();

    }

    bool IsExpired() override
    {
        return _triggledTime < Clock::now();
    }

    Clock::time_point CaculateTriggleTime() override
    {
        using namespace std::chrono_literals;
        if (!IsExpired()) {
            return _triggledTime;
        }
        else {
            if (IsRepeated()) {
                auto deltaDay = (Clock::now() - _triggledTime) / 24h + 1;
                _triggledTime += 24h * deltaDay;
            }
            return _triggledTime;
        }
        //return !IsExpired() ? _triggledTime : (IsRepeated() ? _triggledTime += 24h : _triggledTime);
    }

    Clock::time_point _triggledTime;
};
