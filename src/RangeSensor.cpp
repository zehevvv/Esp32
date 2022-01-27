#include "RangeSensor.hpp"
#include "iostream"
#include "hw.h"
#include "Error.hpp"
#include "Logger.hpp"
#include "Registry.hpp"

using namespace std;

static Adafruit_VL53L0X s_device = Adafruit_VL53L0X();

const string RangeSensor::REGISRTY_NAME_PRINT_RANGE = "print_range";
const string RangeSensor::REGISRTY_NAME_MODE = "range_mode";
const string RangeSensor::REGISRTY_NAME_MIN_RANGE = "min_range";
const string RangeSensor::REGISRTY_NAME_MAX_RANGE = "max_range";

RangeSensor::RangeSensor() :
    m_index(0)    
{
    for (int i = 0; i < NUM_MEASORE; i++)
    {
        m_last_measures[i] = OUT_OF_RANGE;
    }            

    m_print_range = Registry::Instance()->GetKey(REGISRTY_NAME_PRINT_RANGE.c_str(), DEFAULT_PRINT_RANGE);
    m_sensor_mode = (RANGE_SENSOR_MODE)Registry::Instance()->GetKey(REGISRTY_NAME_MODE.c_str(), DEFAULT_MODE);
    m_min_range = Registry::Instance()->GetKey(REGISRTY_NAME_MIN_RANGE.c_str(), DEFAULT_MIN_RANGEE);
    m_max_range = Registry::Instance()->GetKey(REGISRTY_NAME_MAX_RANGE.c_str(), DEFAULT_MAX_RANGEE);
    
    LOG << "Range Sensor init, print range - " << m_print_range 
        << ", mode - " << (int)m_sensor_mode 
        << ", min range - " << m_min_range
        << ", max range - " << m_max_range << "\n";

    if (!s_device.begin())
	{
        TaskManager::Instance()->CriticalError("Failed to boot VL53L0X");
    }

    SetMode(m_sensor_mode);
}

RangeSensor::~RangeSensor()
{
}

void RangeSensor::Run()
{
    if (IsTimePass(DELAY_BETWEEN_MEASURE))
        UpdateRange();
}

void RangeSensor::UpdateRange()
{
    uint16_t range = s_device.readRangeResult();
    if (m_print_range)
        LOG << "range - " << range << "\n";

    if (range > m_max_absolute_range)
        range = OUT_OF_RANGE;

    m_index = (m_index + 1) % NUM_MEASORE;
    m_last_measures[m_index] = range;
}

uint16_t RangeSensor::GetRange()
{
    for (int i = 0; i < NUM_MEASORE; i++)
    {
        if (m_last_measures[i] == OUT_OF_RANGE)
            return OUT_OF_RANGE;
    }
    
    return m_last_measures[m_index];
}

void RangeSensor::SetEnablePringRange(bool enable)
{
    if (m_print_range != enable)
    {
        m_print_range = enable;
        Registry::Instance()->SetKey(REGISRTY_NAME_PRINT_RANGE.c_str(), m_print_range);
    }
}

bool RangeSensor::GetEnablePringRange()
{
    return m_print_range;
}

void RangeSensor::SetMode(RANGE_SENSOR_MODE mode)
{
    if (mode > MODE_HIGH_ACCURACY)
    {
        LOG << "Range mode get invalid mode - " << mode << "\n";
        return;
    }

    if (m_sensor_mode != mode)
    {
        m_sensor_mode = mode;
        LOG << "Range sensor set new mode - " << m_sensor_mode << "\n";
        uint8_t val = m_sensor_mode;
        Registry::Instance()->SetKey(REGISRTY_NAME_MODE.c_str(), val);        
    }    

    s_device.stopRangeContinuous();
    s_device.configSensor((Adafruit_VL53L0X::VL53L0X_Sense_config_t)m_sensor_mode);
    s_device.startRangeContinuous(20);

    if (m_sensor_mode == MODE_LONG_RANGE)
        m_max_absolute_range = 2000;
    else 
        m_max_absolute_range = 1200;

    if (m_max_range > m_max_absolute_range)
        m_max_range = m_max_absolute_range;
}

RANGE_SENSOR_MODE RangeSensor::GetMode()
{
    return m_sensor_mode;
}

uint16_t RangeSensor::GetMinRange()
{
    return m_min_range;
}

void RangeSensor::SetMinRange(uint16_t min_range)
{
    if (min_range > m_max_range)
    {
        LOG << "Error: " << "Set min range of value " << min_range << " failed because it's more than max value of " << m_max_range << "\n";
    }   
    if (m_min_range != min_range)
    {
        m_min_range = min_range;
        Registry::Instance()->SetKey(REGISRTY_NAME_MIN_RANGE.c_str(), m_min_range);
    }
}

uint16_t RangeSensor::GetMaxRange()
{
    return m_max_range;
}

void RangeSensor::SetMaxRange(uint16_t max_range)
{
    if (max_range > m_max_absolute_range)
    {
        LOG << "Error: " << "Set max range of value " << max_range << " failed because the max range allowed is " << m_max_absolute_range << "\n";
    }   
    else if (max_range < m_min_range)
    {
        LOG << "Error: " << "Set max range of value " << max_range << " failed because it's less than min value " << m_min_range << "\n";
    }   
    else if (m_max_range != max_range)
    {
        m_max_range = max_range;
        Registry::Instance()->SetKey(REGISRTY_NAME_MAX_RANGE.c_str(), m_max_range);
    }
}

