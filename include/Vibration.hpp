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
    void SetEnablePrintCycleConfig(bool enable);
    bool GetEnablePrintCycleConfig();
    void SetMinCycle(uint16_t min_cycle);
    uint16_t GetMinCycle();
    void SetMaxCycle(uint16_t max_cycle);
    uint16_t GetMaxCycle();    
    void SetMinPower(uint8_t min_power);
    uint8_t GetMinPower();
    void SetMaxPower(uint8_t max_power);
    uint8_t GetMaxPower();

    static const uint8_t MAX_POWER = 255;
    static const uint8_t NUM_POWER_LEVEL = 10;

private:    
    static const char* REGISRTY_NAME_PRINT_CYCLE_CONFIG;
    static const bool DEFAULT_PRINT_CYCLE_CONFIG = false;
    static const string REGISRTY_NAME_MIN_CYCLE;
    static const uint16_t DEFAULT_MIN_CYCLE = 0;
    static const string REGISRTY_NAME_MAX_CYCLE;
    static const uint16_t DEFAULT_MAX_CYCLE = 300;
    static const string REGISRTY_NAME_MIN_POWER;
    static const uint8_t DEFAULT_MIN_POWER = 0;
    static const string REGISRTY_NAME_MAX_POWER;
    static const uint8_t DEFAULT_MAX_POWER = 10;

    uint8_t m_min_power;
    uint8_t m_max_power;    
    uint16_t m_min_cycle;
    uint16_t m_max_cycle;
    bool m_print_cycle_config;

    void SetPower(uint8_t power);
};

