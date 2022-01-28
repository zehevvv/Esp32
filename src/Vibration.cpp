#include "Vibration.hpp"
#include "hw.h"
#include "iostream"
#include "RangeSensor.hpp"
#include "Registry.hpp"
#include "Logger.hpp"

const string Vibration::REGISRTY_NAME_MIN_POWER = "min_power";
const string Vibration::REGISRTY_NAME_MAX_POWER = "max_power";
const string Vibration::REGISRTY_NAME_PRINT_CYCLE_CONFIG = "print_cycle";
const string Vibration::REGISRTY_NAME_MIN_CYCLE = "min_cycle";
const string Vibration::REGISRTY_NAME_MAX_CYCLE = "max_cycle";
const string Vibration::REGISRTY_NAME_ENABLE_VIBRATION = "enable_vib";

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

    m_min_power = Registry::Instance()->GetKey(REGISRTY_NAME_MIN_POWER.c_str(), DEFAULT_MIN_POWER);
    m_max_power = Registry::Instance()->GetKey(REGISRTY_NAME_MAX_POWER.c_str(), DEFAULT_MAX_POWER);
    m_print_cycle_config = Registry::Instance()->GetKey(REGISRTY_NAME_PRINT_CYCLE_CONFIG.c_str(), DEFAULT_PRINT_CYCLE_CONFIG);
    m_min_cycle = Registry::Instance()->GetKey(REGISRTY_NAME_MIN_CYCLE.c_str(), DEFAULT_MIN_CYCLE);
    m_max_cycle = Registry::Instance()->GetKey(REGISRTY_NAME_MAX_CYCLE.c_str(), DEFAULT_MAX_CYCLE);
    m_enable_vibration = Registry::Instance()->GetKey(REGISRTY_NAME_ENABLE_VIBRATION.c_str(), DEFAULT_ENABLE_VIBRATION);
    
    LOG <<  "Vibration init:\n" <<
            "Min power - " << m_min_power << "\n" <<
            "Max power - " << m_max_power << "\n" <<
            "Min cycle - " << m_min_cycle << "\n" << 
            "Max cycle - " << m_max_cycle << "\n"
            "Enable bibration - " << m_enable_vibration << "\n";
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
            uint16_t power = map(current_range, min_range, max_range, m_min_power, m_max_power);
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
    if (m_enable_vibration)
    {
        uint8_t val =  power = (MAX_POWER / 10) * power;
        analogWrite(MOTOR_PIN_PWM_A, val);
    }
    else
    {
        analogWrite(MOTOR_PIN_PWM_A, 0);
    }
}

void Vibration::SetMaxPower(uint8_t max_power)
{
    if (max_power > NUM_POWER_LEVEL)
    {
        LOG << "Error: Failed set max power because power level to high, " << max_power << "\n";
        return;
    }
    else if (max_power < m_min_power)
    {
        LOG << "Error: Failed set max power because max power " << max_power << " is less than min power " << m_min_power << "\n";
    }
    else if (max_power != m_max_power)
    {
        m_max_power = max_power;
        Registry::Instance()->SetKey(REGISRTY_NAME_MAX_POWER.c_str(), m_max_power);
    }
}

uint8_t Vibration::GetMaxPower()
{
    return m_max_power;
}

void Vibration::SetMinPower(uint8_t min_power)
{
    if (min_power > m_max_power)
    {
        LOG << "Error: Failed set min power because min power " << min_power << " is more than max power " << m_max_power << "\n";
    }
    else if (min_power != m_min_power)
    {
        m_min_power = min_power;
        Registry::Instance()->SetKey(REGISRTY_NAME_MIN_POWER.c_str(), m_min_power);
    }
}

uint8_t Vibration::GetMinPower()
{
    return m_min_power;
}

void Vibration::SetEnablePrintCycleConfig(bool enable)
{
    if (m_print_cycle_config != enable)
    {
        m_print_cycle_config = enable;
        Registry::Instance()->SetKey(REGISRTY_NAME_PRINT_CYCLE_CONFIG.c_str(), m_print_cycle_config);
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

void Vibration::SetEnableVibration(bool enable)
{
    if (m_enable_vibration != enable)
    {
        m_enable_vibration = enable;
        Registry::Instance()->SetKey(REGISRTY_NAME_ENABLE_VIBRATION.c_str(), m_enable_vibration);
    }
}

bool Vibration::GetEnableVibration()
{
    return m_enable_vibration;
}