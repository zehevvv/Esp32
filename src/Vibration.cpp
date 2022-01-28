#include "Vibration.hpp"
#include "hw.h"
#include "iostream"
#include "RangeSensor.hpp"
#include "Registry.hpp"
#include "Logger.hpp"

const string Vibration::REGISRTY_NAME_POWER_LEVEL = "power_level";
const char* Vibration::REGISRTY_NAME_PRINT_CYCLE_CONFIG = "print_cycle";
const string Vibration::REGISRTY_NAME_MIN_CYCLE = "min_cycle";
const string Vibration::REGISRTY_NAME_MAX_CYCLE = "max_cycle";

Vibration::Vibration() :
    m_max_power(MAX_POWER)
{
    pinMode(MOTOR_PIN_IN_A_1, OUTPUT);
    pinMode(MOTOR_PIN_IN_A_2, OUTPUT);
    pinMode(MOTOR_PIN_STANDBY, OUTPUT);
    pinMode(MOTOR_PIN_PWM_A, OUTPUT);

    digitalWrite(MOTOR_PIN_IN_A_1, HIGH);
	digitalWrite(MOTOR_PIN_IN_A_2, LOW);
	digitalWrite(MOTOR_PIN_STANDBY, HIGH);
	analogWrite(MOTOR_PIN_PWM_A, 0);

    m_power_level = Registry::Instance()->GetKey(REGISRTY_NAME_POWER_LEVEL.c_str(), DEFAULT_POWER_LEVEL);
    m_print_cycle_config = Registry::Instance()->GetKey(REGISRTY_NAME_PRINT_CYCLE_CONFIG, DEFAULT_PRINT_CYCLE_CONFIG);
    m_min_cycle = Registry::Instance()->GetKey(REGISRTY_NAME_MIN_CYCLE.c_str(), DEFAULT_MIN_CYCLE);
    m_max_cycle = Registry::Instance()->GetKey(REGISRTY_NAME_MAX_CYCLE.c_str(), DEFAULT_MAX_CYCLE);
    
    m_max_power = (MAX_POWER / 10) * m_power_level;
    LOG <<  "Vibration level - " << (int)m_power_level << ", max power - " << (int)m_max_power << 
            ", Min cycle - " << m_min_cycle << ", Max cycle - " << m_max_cycle << "\n";
}

Vibration::~Vibration()
{
}

void Vibration::Run()
{
    static int next_cycle = 1;
    static bool is_vibration_off = false;
    if (IsTimePass(next_cycle))
    {
        uint16_t current_range = RangeSensor::Instance()->GetRange();
        if (current_range != RangeSensor::OUT_OF_RANGE)
        {
            uint16_t min_range = RangeSensor::Instance()->GetMinRange();
            uint16_t max_range = RangeSensor::Instance()->GetMaxRange();
            next_cycle  = map(current_range, min_range, max_range, m_min_cycle, m_max_cycle);
            uint16_t power  = map(current_range, min_range, max_range, 0, m_max_power);
            power = MAX_POWER - power;

            if (m_print_cycle_config)
                LOG << "range = " << current_range << ", next cycle = " << next_cycle << ", power = " << power << "\n";

            if (is_vibration_off)
            {
                SetPower(power);
                is_vibration_off = false;
            }
            else
            {
                SetPower(0);
                is_vibration_off = true;
            }
        }
        else
        {
            SetPower(0);
            next_cycle = 1;
        }
    }
}

void Vibration::SetPower(uint8_t power)
{
    analogWrite(MOTOR_PIN_PWM_A, power);
}

void Vibration::SetPowerLevel(uint8_t power_level)
{
    if (power_level > NUM_POWER_LEVEL)
    {
        LOG << "Error: Get power level to high, " << power_level << "\n";
        return;
    }

    if (power_level == m_power_level)
        return;

    m_power_level = power_level;
    Registry::Instance()->SetKey(REGISRTY_NAME_POWER_LEVEL.c_str(), m_power_level);
    m_max_power = (MAX_POWER / 10) * m_power_level;
}

uint8_t Vibration::GetPowerLevel()
{
    return m_power_level;
}


void Vibration::SetEnablePrintCycleConfig(bool enable)
{
    if (m_print_cycle_config != enable)
    {
        m_print_cycle_config = enable;
        Registry::Instance()->SetKey(REGISRTY_NAME_PRINT_CYCLE_CONFIG, m_print_cycle_config);
    }
}

bool Vibration::GetEnablePrintCycleConfig()
{
    return m_print_cycle_config;
}

void Vibration::SetMinCycle(uint16_t min_cycle)
{
    if (m_min_cycle != min_cycle)
    {
        m_min_cycle = min_cycle;
        Registry::Instance()->SetKey(REGISRTY_NAME_MIN_CYCLE.c_str(), m_min_cycle);
    }
}

uint16_t Vibration::GetMinCycle()
{
    return m_min_cycle;
}

void Vibration::SetMaxCycle(uint16_t max_cycle)
{
    if (m_max_cycle != max_cycle)
    {
        m_max_cycle = max_cycle;
        Registry::Instance()->SetKey(REGISRTY_NAME_MAX_CYCLE.c_str(), m_max_cycle);
    }
}

uint16_t Vibration::GetMaxCycle()
{
    return m_max_cycle;
}
