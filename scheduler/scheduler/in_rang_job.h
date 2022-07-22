#pragma once
#include "job.h"
#include <vector>
#include <map>


class InRangeJob : public Job
{
public:
    InRangeJob(const std::string& id, Function&& work)
        : Job(id, work, true)
    {}

    void Push(const std::vector<std::string>& times)
    {
        for (const auto& time : times){
            _triggleTimes.emplace(FormatTimePoint(time), time);
        }
    }

    Clock::time_point GetTriggleTime() override
    {
        return CaculateTriggleTime();
    }

    void ResetTriggledTime() override
    {
        decltype(_triggleTimes) copy;
        copy.swap(_triggleTimes);
        for (const auto& item : copy) {
            _triggleTimes.emplace(FormatTimePoint(item.second), item.second);
        }
    }

protected:
    bool IsExpired() override
    {
        auto expiredEndIter = _triggleTimes.upper_bound(Clock::now());
        return expiredEndIter == _triggleTimes.end();
    }

    Clock::time_point CaculateTriggleTime() override
    {
        auto removeEndIter = _triggleTimes.upper_bound(Clock::now());
        _triggleTimes.erase(_triggleTimes.begin(), removeEndIter);
        auto first = _triggleTimes.begin();
        if (first == _triggleTimes.end()) {
            _valid = false;
            _repeated = false;
            return Clock::time_point(Clock::duration(0));
        }

        return first->first;
    }

    static Clock::time_point FormatTimePoint(const std::string time)
    {
        using namespace std::chrono_literals;

        auto tm = cron::utils::to_tm(time);
        return Clock::from_time_t(cron::utils::tm_to_time(tm));
    }
    
private:
    std::multimap<Clock::time_point, std::string> _triggleTimes;
};
