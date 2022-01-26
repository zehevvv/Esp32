#pragma once

#include "Adafruit_VL53L0X.h"
#include "TaskManager.hpp"


typedef enum  
{
    MODE_DEFAULT = 0,
    MODE_LONG_RANGE,
    MODE_HIGH_SPEED,
    MODE_HIGH_ACCURACY

} RANGE_SENSOR_MODE;

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
    void SetMode(RANGE_SENSOR_MODE mode);
    RANGE_SENSOR_MODE GetMode();
    uint16_t GetMaxRange();

    static const uint16_t OUT_OF_RANGE = 0xffff;
    static const uint16_t NUM_MEASORE = 3;
    static const uint16_t DELAY_BETWEEN_MEASURE = 25;

private:
    void UpdateRange();

    uint16_t            m_last_measures[NUM_MEASORE];
    uint8_t             m_index;
    bool                m_print_range;
    RANGE_SENSOR_MODE   m_sensor_mode;
    uint16_t            m_max_range;

    static const string     REGISRTY_NAME_PRINT_RANGE;
    static const bool       DEFAULT_PRINT_RANGE = false;
    static const string     REGISRTY_NAME_MODE;
    static const uint8_t    DEFAULT_MODE = MODE_DEFAULT;
};