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
    void SetEnablePrintCycleConfig(bool enable);
    bool GetEnablePrintCycleConfig();
    void SetMinCycle(uint16_t min_cycle);
    uint16_t GetMinCycle();
    void SetMaxCycle(uint16_t max_cycle);
    uint16_t GetMaxCycle();    

    static const uint8_t MAX_POWER = 255;
    static const uint8_t NUM_POWER_LEVEL = 10;

private:    
    static const string REGISRTY_NAME_POWER_LEVEL;
    static const uint8_t DEFAULT_POWER_LEVEL = 10;
    static const char* REGISRTY_NAME_PRINT_CYCLE_CONFIG;
    static const bool DEFAULT_PRINT_CYCLE_CONFIG = false;
    static const string REGISRTY_NAME_MIN_CYCLE;
    static const uint16_t DEFAULT_MIN_CYCLE = 0;
    static const string REGISRTY_NAME_MAX_CYCLE;
    static const uint16_t DEFAULT_MAX_CYCLE = 0;

    uint8_t m_max_power;    
    uint8_t m_power_level;
    uint16_t m_min_cycle;
    uint16_t m_max_cycle;
    bool m_print_cycle_config;

    void SetPower(uint8_t power);
};

