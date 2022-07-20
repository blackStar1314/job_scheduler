#include "job_scheduler.h"
#include "time_utils.h"
#include "logger.h"

JobScheduler::JobScheduler(int maxParallel)
    : _done(false)
{
}

JobScheduler::~JobScheduler()
{
    Shutdown();
}

void JobScheduler::Startup()
{
    _thread = std::thread(std::bind(&JobScheduler::ProcessJob, this));
}

void JobScheduler::Shutdown()
{
    _done.exchange(true);
    _done.store(true);
    _event.Wake();
    // wait traggled job continue working 
    if (_thread.joinable()) {
        _thread.join();
    }
    ClearAll();
    _done.exchange(false);
}

void JobScheduler::ReStartup()
{
    const auto jobs = CopyJobs();

    Shutdown();
    Startup();

    for (const auto& job : jobs) {
        job->ResetTriggledTime();
        Add(job);
    }

}

void JobScheduler::Add(std::shared_ptr<Job> job)
{
    AddImpl(job);
    _event.Wake();
}

void JobScheduler::Delete(const std::string& id)
{
    Cancel(id);
    _event.Wake();
}

size_t JobScheduler::Jobs()
{
    std::lock_guard<std::mutex> lck(_lock);
    return _jobs.size();
}

std::multimap<Job::Clock::time_point, std::shared_ptr<Job>>::iterator JobScheduler::Find(const std::string& id)
{
    for (auto iter = _jobs.begin(); iter != _jobs.end(); ++iter) {
        if (iter->second->GetId() == id) {
            return iter;
        }
    }
    return _jobs.end();
}

void JobScheduler::AddImpl(std::shared_ptr<Job> job)
{
    std::lock_guard<std::mutex> lck(_lock);
    _jobs.emplace(job->GetTriggleTime(), job);
}

bool JobScheduler::Cancel(const std::string& id)
{
    std::lock_guard<std::mutex> lck(_lock);
    auto iter = Find(id);
    if (iter != _jobs.end()) {
        iter->second->Enable(false);
        return true;
    }
    return false;
}

void JobScheduler::ClearAll()
{
    std::lock_guard<std::mutex> lck(_lock);
    _jobs.clear();
}

void JobScheduler::ProcessJob()
{
    while (!_done.load()) {

        {
            if (_jobs.empty()) {
                _event.Wait();
            }
            else {
                const auto tp = _jobs.begin()->first;
                const auto str = TimeTotm(tp.time_since_epoch().count());
                LOGS << "Next job triggled  timepoint : " << str << "\n";
                
                _event.WaitUntil(tp);
            }
        }

        {
            std::lock_guard<std::mutex> lck(_lock);
            decltype(_jobs) repeatedJobs;
            auto jobsOfTraggledEnd = _jobs.upper_bound(Job::Clock::now());
            for (auto iter = _jobs.begin(); iter != jobsOfTraggledEnd; ++iter) {
                auto& job = iter->second;

                if (job->IsValid() &&
                    job->IsEnable()) {

                    // blocking
                    job->Work();

                    if (job->IsRepeated()) {
                        repeatedJobs.emplace(job->GetTriggleTime(), job);
                    }
                }
            }

            _jobs.erase(_jobs.begin(), jobsOfTraggledEnd);
            for (auto iter = repeatedJobs.begin(); iter != repeatedJobs.end(); ++iter) {
                _jobs.emplace(iter->first, iter->second);
            }
        }
    }
}

std::vector<std::shared_ptr<Job>> JobScheduler::CopyJobs()
{
    std::vector<std::shared_ptr<Job>> jobs;
    std::lock_guard<std::mutex> lck(_lock);
    for (auto iter = _jobs.cbegin(); iter != _jobs.cend(); ++iter) {
        jobs.push_back(iter->second);
    }
    return jobs;
}
