#pragma once

#include "TaskManager.hpp"

class OTA : public Task<OTA>
{
friend Singleton<OTA>;
friend TaskManager;
protected:
    OTA();
    ~OTA();
    void Run();
};
