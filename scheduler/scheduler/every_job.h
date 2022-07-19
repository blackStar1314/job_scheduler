#pragma once
#include "in_job.h"


class EveryJob : public InJob
{
public:

    explicit EveryJob(const std::string& id, const std::string& time, Function&& work)
        : InJob(id, time, std::move(work), true)
    {}
};
