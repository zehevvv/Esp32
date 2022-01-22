#pragma once

#include "TaskManager.hpp"

class TaskLed : public Task<TaskLed>
{
friend Singleton<TaskLed>;
private:
    TaskLed();
    ~TaskLed();
    void Run();
};

