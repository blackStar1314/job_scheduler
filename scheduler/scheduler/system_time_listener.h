#pragma once
#include <Windows.h>
#include <future>


class SystemTimeListener
{
public:
    using TimeChangedNotification = std::function<void()>;

    static SystemTimeListener& Instance()
    {
        static SystemTimeListener instance;

        return instance;
    }

    void SetNotification(const TimeChangedNotification& notification)
    {
        _notification = notification;
    }

    void Start()
    {
        _dumpMessageWorker = std::async(std::launch::async, std::bind(&SystemTimeListener::DumpMessage, this));
    }

    void Stop()
    {
        SendMessageW(_messageHwnd, WM_QUIT, 0, 0);
        if (_dumpMessageWorker.valid()) {
            _dumpMessageWorker.get();
        }
        UnRegisterClassImpl();
    }


    ~SystemTimeListener()
    {
        Stop();
    }


private:
    SystemTimeListener()
        : _messageHwnd(nullptr)
    {}

    SystemTimeListener(const SystemTimeListener&) = delete;
    SystemTimeListener& operator=(const SystemTimeListener&) = delete;

    bool CreateMessageWindow()
    {
        if (!RegisterClassImp()) {
            return false;
        }

        if (!CreateWindowImpl()) {
            return false;
        }

        return true;
    }

    static bool RegisterClassImp()
    {
        WNDCLASSEXW wcex = {};

        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc = &SystemTimeListener::Wndproc;
        wcex.lpszClassName = kClassName;

        return 0 != ::RegisterClassExW(&wcex);

    }

    static void UnRegisterClassImpl()
    {
        ::UnregisterClassW(kClassName, ::GetModuleHandleW(nullptr));
    }

    bool CreateWindowImpl()
    {
        _messageHwnd = CreateWindowEx(0, kClassName, L"", WS_POPUP, 0, 0, 0, 0,
            nullptr,
            nullptr,
            ::GetModuleHandleW(nullptr), 
            this);
        return _messageHwnd != nullptr;
    }

    bool DumpMessage()
    {
        if (!CreateMessageWindow()) return false;

        MSG msg;
        while (::GetMessageW(&msg, nullptr, 0, 0)) {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
        return true;
    }

    LRESULT CALLBACK HandleMessage(
        HWND hwnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam){

        if(msg == WM_TIMECHANGE){
            if (this->_notification) {
                this->_notification();
            }
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    
    static LRESULT CALLBACK Wndproc(
        HWND hwnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam 
    )
    {
        //if (msg == WM_TIMECHANGE) {
        //    std::cerr << "wm time change\n";
        //}
        //return DefWindowProcW(hwnd, msg, wParam, lParam);
        static SystemTimeListener* self = nullptr;

        if (msg == WM_NCCREATE) {
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            self = static_cast<SystemTimeListener*>(lpcs->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(self));
        }
        if (!self) {
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }

        return self->HandleMessage(hwnd, msg, wParam, lParam);
    }
private:
    static constexpr auto kClassName = L"systemTimeListener";
    HWND _messageHwnd;
    TimeChangedNotification _notification;

    std::future<bool> _dumpMessageWorker;
};
