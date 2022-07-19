#pragma once
#include <memory>
#include <map>
#include "job.h"
#include "cron_job.h"
#include "in_job.h"
#include "every_job.h"
#include "event.h"

class JobScheduler
{
public:
    explicit JobScheduler(int maxParallel);
    ~JobScheduler();
    
    // no repeated
    // work at time
    template<typename Callable, typename... Args>
    static std::shared_ptr<Job> At(const std::string& time, const std::string& id, Callable&& f, Args &&... args)
    {
        auto fn = std::bind(std::forward<Callable>(f), std::forward<Args>(args)...);
        auto job = std::make_shared<InJob>(id, time, fn);
        if (!job->IsValid()) {
            job.reset();
        }
        return job;
    }

    // repeated
    // every day at time

    template<typename Callable, typename... Args>
    static std::shared_ptr<Job> Every(const std::string& time, const std::string& id, Callable&& f, Args &&... args)
    {
        auto fn = std::bind(std::forward<Callable>(f), std::forward<Args>(args)...);
        auto job = std::make_shared<EveryJob>(id, time, fn);
        if (!job->IsValid()) {
            job.reset();
        }
        return job;
    }
    
    // repeated
    // work in loop 
    template<typename Callable, typename... Args>
    static std::shared_ptr<Job> Cron(const std::string& expression, const std::string& id, Callable&& f, Args &&... args)
    {
        auto fn = std::bind(std::forward<Callable>(f), std::forward<Args>(args)...);
        auto job = std::make_shared<CronJob>(expression, id, fn);
        if (!job->IsValid()) {
            job.reset();
       }
        return job;
    }

    void Startup();
    void Shutdown();
    void ReStartup();
    void Add(std::shared_ptr<Job> job);
    void Delete(const std::string& id);
    size_t Jobs();
private:
    JobScheduler(const JobScheduler&) = delete;
    JobScheduler& operator= (const JobScheduler&) = delete;

    std::multimap<Job::Clock::time_point, std::shared_ptr<Job>>::iterator Find(const std::string& id);
    void AddImpl(std::shared_ptr<Job> job);
    bool Cancel(const std::string& id);
    void ClearAll();
    void ProcessJob();

    std::vector<std::shared_ptr<Job>> CopyJobs();
private:

    std::multimap<Job::Clock::time_point, std::shared_ptr<Job>> _jobs;

   std::atomic_bool _done;
    std::thread _thread;
    std::mutex _lock;
    Event _event;
};
