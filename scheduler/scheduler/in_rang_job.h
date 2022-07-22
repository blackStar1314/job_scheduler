#pragma once
#include "job.h"
#include <vector>
#include <map>


class InRangeJob : public Job
{
public:
    InRangeJob(const std::string& id, Function&& work, bool once = true)
        : Job(id, work, true)
        , _once(once)
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
    }

protected:
    bool IsExpired() override
    {
        auto expiredEndIter = _triggleTimes.upper_bound(Clock::now());
        return expiredEndIter == _triggleTimes.end();
    }

    Clock::time_point CaculateTriggleTime() override
    {
        decltype(_triggleTimes)::iterator first = _triggleTimes.begin();
        if (!_once) {
            first = std::find_if(_triggleTimes.begin(), _triggleTimes.end(),
                [](const decltype(_triggleTimes)::value_type& e) {
                    return e.first >= Clock::now();
                });
            if (first == _triggleTimes.end()) {
                first = _triggleTimes.begin();
            }
        }
        else {
            auto removeEndIter = _triggleTimes.upper_bound(Clock::now());
            _triggleTimes.erase(_triggleTimes.begin(), removeEndIter);
            first = _triggleTimes.begin();
        }

        if (first == _triggleTimes.end()) {
            if (_once) _repeated = false;
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
    bool _once;
    std::multimap<Clock::time_point, std::string> _triggleTimes;
};
