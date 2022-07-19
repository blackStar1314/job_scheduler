// scheduler.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include "job_scheduler.h"
#include "croncpp.h"
#include "system_time_listener.h"


static std::ofstream gLog;

static std::string PrintTime()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);

    return "[" + cron::utils::to_string(*now) + "]";
}

#define LOG_DEBUG std::cout << PrintTime()
#define LOG std::cerr << PrintTime()

static void OnTraggled(const std::string& name)
{
    LOG_DEBUG << "name : " << name << "\n";
    LOG << "name : " << name << "\n";
}


static void TimeTotm(int64_t tick)
{
    using namespace std::chrono_literals;
    auto duration = std::chrono::system_clock::duration(tick);

    auto tp = std::chrono::system_clock::time_point(duration);
    auto time = std::chrono::system_clock::to_time_t(tp);
    tm tm;
    cron::utils::time_to_tm(&time, &tm);
    std::cout << cron::utils::to_string(tm) << std::endl;
}

static void PrintJob(std::shared_ptr<Job> job)
{
    LOG_DEBUG << job->GetId() << "\t"
        << " valid : " << job->IsValid() << "\t"
        << " enable : " << job->IsEnable() << "\t"
        << " repeated : " << job->IsRepeated() << "\n";
    LOG << job->GetId() << "\t"
        << " valid : " << job->IsValid() << "\t"
        << " enable : " << job->IsEnable() << "\t"
        << " repeated : " << job->IsRepeated() << "\n";
}

int main()
{
    gLog.open("./scheduler.log", std::ios::app | std::ios::out);
    std::cerr.set_rdbuf(gLog.rdbuf());

    LOG_DEBUG << "Starup...\n";
    LOG << "Starup...\n";
    JobScheduler scheduler1(1), scheduler2(1);
    auto notification = []() {
        LOG_DEBUG << "System time changed.\n";
        TimeTotm(std::chrono::system_clock::now().time_since_epoch().count());
    };
    scheduler1.Startup();
    // create job
    // 0 : 星期天
    auto job = JobScheduler::Cron("0 0 9 ? * 0-3,5", "1", OnTraggled, "Job-1: Mon, Tue, Wed, Fri, Sun of weekly at 09:00:00");
    if (job) {
        PrintJob(job);
        scheduler1.Add(job);
    }

    job = JobScheduler::Cron("59 59 23 ? * 4,6", "2", OnTraggled, "Job-2: Thu, Sta of weekly at 23:59:59");
    if (job) {
        PrintJob(job);
        scheduler1.Add(job);
    }

    job = JobScheduler::Cron("22 22 22 ? * 4,6", "3", OnTraggled, "Job-3: Thu, Sta of weekly at 22:22:22");
    if (job) {
        PrintJob(job);
        scheduler1.Add(job);
    }

    job = JobScheduler::Every("2022-07-15 14:00:00", "4", OnTraggled, "Job-4: Every day at 14:00:00 from 2022-07-15");
    if (job) {
        PrintJob(job);
        scheduler1.Add(job);
    }

    job = JobScheduler::At("2022-07-19 12:30:00", "5", OnTraggled, "Job-5:  At 12:30:00  2022-07-19, only one");
    if (job) {
        PrintJob(job);
        scheduler1.Add(job);
    }

    LOG_DEBUG << "current jobs : " << scheduler1.Jobs() << "\n";
    LOG << "current jobs : " << scheduler1.Jobs() << "\n";
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}
