﻿// scheduler.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "job_scheduler.h"
#include "system_time_listener.h"
#include "time_utils.h"
#include "logger.h"

static void OnTraggled(const std::string& name)
{
    LOG << "name : " << name << "\n";
}


static void PrintJob(std::shared_ptr<Job> job)
{
    LOG << job->GetId() << "\t"
        << " valid : " << job->IsValid() << "\t"
        << " enable : " << job->IsEnable() << "\t"
        << " repeated : " << job->IsRepeated() << "\n";
}

int main()
{
    Logger::Instance().InitLogger("./scheduler.log");
    LOG << "Starup...\n";
    JobScheduler scheduler1(1), scheduler2(1);
    auto notification = [&scheduler1]() {
        LOG << "System time changed, restartup scheduler : " << TimeTotm(std::chrono::system_clock::now().time_since_epoch().count()) << "\n";

        scheduler1.ReStartup();
        LOG << "current jobs : " << scheduler1.Jobs() << "\n";
    };
    SystemTimeListener::Instance().SetNotification(notification);
    SystemTimeListener::Instance().Start();
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

    LOG << "current jobs : " << scheduler1.Jobs() << "\n";
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}
