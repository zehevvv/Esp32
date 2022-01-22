#pragma once

#include <analogWrite.h>
#include "TaskManager.hpp"
#include "string"

using namespace std;

class Vibration : public Task<Vibration>
{
friend Singleton<Vibration>;
protected:
    Vibration();
    ~Vibration();
    void Run();

public:
    void SetPowerLevel(uint8_t max_power);
    uint8_t GetPowerLevel();

    static const uint8_t MAX_POWER = 255;
    static const uint8_t NUM_POWER_LEVEL = 10;

private:    
    static const string REGISRTY_NAME_POWER_LEVEL;
    static const uint8_t DEFAULT_POWER_LEVEL = 10;

    uint8_t m_current_max_power;
    uint8_t m_power_level;

    void SetPower(uint8_t power);
};

