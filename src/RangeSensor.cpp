#include "RangeSensor.hpp"
#include "iostream"
#include "hw.h"
#include "Error.hpp"
#include "Logger.hpp"

using namespace std;

static Adafruit_VL53L0X s_device = Adafruit_VL53L0X();

RangeSensor::RangeSensor() :
    m_index(0)
{
    for (int i = 0; i < NUM_MEASORE; i++)
    {
        m_last_measures[i] = OUT_OF_RANGE;
    }        
    
    if (!s_device.begin())
	{
        TaskManager::Instance()->CriticalError("Failed to boot VL53L0X");
    }

    s_device.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_DEFAULT);

    if (!s_device.startRangeContinuous())
	{
        TaskManager::Instance()->CriticalError("Failed to start continuous mode");
    }
}

RangeSensor::~RangeSensor()
{
}

void RangeSensor::UpdateRange()
{
    uint16_t range = s_device.readRangeResult();
    m_index = (m_index + 1) % NUM_MEASORE;
    if (range > MAX_RANGE_MM)
        range = OUT_OF_RANGE;
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

void RangeSensor::Run()
{
    if (IsTimePass(DELAY_BETWEEN_MEASURE))
        UpdateRange();
}