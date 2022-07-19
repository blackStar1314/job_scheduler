#pragma once
#include <string>
#include <chrono>
#include <functional>

class Job
{
public:

    using Function = std::function<void()>;
    using Clock = std::chrono::system_clock;
    
    explicit Job(const std::string& id, Function& work, bool repeated = false)
        : _id(id)
        , _work(work)
        , _repeated(repeated)
        , _valid(true)
        , _enable(true)
    {}
    virtual ~Job() = default;

    virtual Clock::time_point GetTriggleTime() = 0;

    const std::string& GetId() const { return _id; }
    bool IsRepeated() const { return _repeated; }
    bool IsValid() const { return _valid; }
    void Enable(bool enable) { _enable = enable; }
    bool IsEnable() const { return _enable; }
    void Work() { _work(); }

protected:
    virtual bool IsExpired() = 0;
    virtual Clock::time_point CaculateTriggleTime() = 0;

    std::string _id;
    bool        _repeated;
    bool        _valid;
    bool        _enable;
    Function    _work;
};
