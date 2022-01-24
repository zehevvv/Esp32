#pragma once

#include "TaskManager.hpp"

class TaskLed : public Task<TaskLed>
{
friend Singleton<TaskLed>;
private:
    TaskLed();
    ~TaskLed();
    void Run();

    bool m_print_alive;

    static const string     REGISRTY_NAME_PRINT_ALIVE;
    static const bool       DEFAULT_PRINT_ALIVE = true;

public:
    void SetEnablePringAlive(bool enable);
    bool GetEnablePringAlive();
};

