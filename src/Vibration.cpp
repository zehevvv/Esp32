#include "Vibration.hpp"
#include "hw.h"
#include "iostream"
#include "RangeSensor.hpp"
#include "Registry.hpp"
#include "Logger.hpp"

const string Vibration::REGISRTY_NAME_POWER_LEVEL = "power_level";

Vibration::Vibration() :
    m_current_max_power(MAX_POWER)
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
    m_current_max_power = (MAX_POWER / 10) * m_power_level;
    LOG << "vibration level " << (int)m_power_level << ", max power " << (int)m_current_max_power << "\n";
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
        uint16_t distance = RangeSensor::Instance()->GetRange();
        if (distance != RangeSensor::OUT_OF_RANGE)
        {
            next_cycle  = map(distance, 0, RangeSensor::MAX_RANGE_MM, 0, 300);
            uint16_t power  = map(distance, 0, RangeSensor::MAX_RANGE_MM, 0, m_current_max_power);
            power = MAX_POWER - power;
            // LOG << "range = " << distance << ", next cycle = " << next_cycle << ", power = " << power << "\n";

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
    m_current_max_power = (MAX_POWER / 10) * m_power_level;
}

uint8_t Vibration::GetPowerLevel()
{
    return m_power_level;
}