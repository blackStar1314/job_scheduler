#pragma once
#include <mutex>
#include <condition_variable>

class Event
{
public:
    using Clock = std::chrono::system_clock;
public:
    Event()
        : _waked(false)
    {}

    void WaitUntil(const Clock::time_point& time)
    {
        std::unique_lock<std::mutex> lck(_mtx);
        _cv.wait_until(lck, time, [this]() {
            return _waked;
            });
        _waked = false;
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lck(_mtx);
        _cv.wait(lck, [this]() {
            return _waked;
            });
        _waked = false;
    }

    void Wake()
    {
        std::lock_guard<std::mutex> lck(_mtx);
        _waked = true;
        _cv.notify_one();
    }


private:
    bool _waked;
    std::mutex _mtx;
    std::condition_variable _cv;
};
