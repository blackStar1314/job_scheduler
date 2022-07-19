#pragma once
#include <cassert>
#include "croncpp.h"
#include "job.h"


class CronJob : public Job
{
public:
    explicit CronJob(const std::string& expression, const std::string& id, Function&& work)
        : Job(id, work, true)
        , _expression(expression)
    {
        _valid = IsValidExpression(expression);
    }

    Clock::time_point GetTriggleTime() override
    {
        assert(IsValid() && "cron expression is illegal");

        return CaculateTriggleTime();
    }

private:

    static bool IsValidExpression(const std::string& expr)
    {
        try {
            cron::make_cron(expr);
        }
        catch (const cron::bad_cronexpr&) {
            return false;
        }
        return true;
    }

    bool IsExpired() override
    {
        return CaculateTriggleTime() < Clock::now();
    }

    Clock::time_point CaculateTriggleTime() override
    {
        return cron::cron_next(cron::make_cron(_expression), Clock::now());
    }

    std::string _expression;
};
