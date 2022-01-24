#pragma once

#include "Adafruit_VL53L0X.h"
#include "TaskManager.hpp"


class RangeSensor : public Task<RangeSensor>
{
friend Singleton<RangeSensor>;
protected:
    RangeSensor();
    ~RangeSensor();
    void Run();

public:
    uint16_t GetRange();    
    void SetEnablePringRange(bool enable);
    bool GetEnablePringRange();

    static const uint16_t OUT_OF_RANGE = 0xffff;
    static const uint16_t NUM_MEASORE = 3;
    static const uint16_t DELAY_BETWEEN_MEASURE = 25;
    static const uint16_t MAX_RANGE_MM = 1000;
    static const Adafruit_VL53L0X::VL53L0X_Sense_config_t SENSOR_MODE = Adafruit_VL53L0X::VL53L0X_SENSE_DEFAULT;

private:
    void UpdateRange();

    uint16_t            m_last_measures[NUM_MEASORE];
    uint8_t             m_index;
    bool                m_print_range;

    static const string     REGISRTY_NAME_PRINT_RANGE;
    static const bool       DEFAULT_PRINT_RANGE = false;
};