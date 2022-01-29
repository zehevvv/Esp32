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

    static const int RESET_MAGIC = 0x23456789;
    static const int MAX_ALLOWED_RESET = 3;

public:
    bool IsSystemStable();
};
