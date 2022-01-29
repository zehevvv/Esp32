#pragma once

#include "TaskManager.hpp"

class LedTask : public Task<LedTask>
{
friend Singleton<LedTask>;
private:
    LedTask();
    ~LedTask();
    void Run();

    bool    m_print_alive;
    int     m_blink_time;

    static const string     REGISRTY_NAME_PRINT_ALIVE;
    static const bool       DEFAULT_PRINT_ALIVE = true;
    static const int        BLINK_TIME_STABLE_SYSTEM = 1000;
    static const int        BLINK_TIME_UNSTABLE_SYSTEM = 100;

public:
    void SetEnablePringAlive(bool enable);
    bool GetEnablePringAlive();
    void SetUnstableSystemBlink();
};

