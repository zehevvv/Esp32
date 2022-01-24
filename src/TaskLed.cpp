#include "TaskLed.hpp"
#include "hw.h"
#include "iostream"
#include "Logger.hpp"
#include "Registry.hpp"

using namespace std;

const string TaskLed::REGISRTY_NAME_PRINT_ALIVE = "print_alive";

TaskLed::TaskLed()
{
    pinMode(LED_PIN, OUTPUT);
    m_start_waiting = 0;
    m_time_to_wait = 0;    

    m_print_alive = Registry::Instance()->GetKey(REGISRTY_NAME_PRINT_ALIVE.c_str(), DEFAULT_PRINT_ALIVE);
}

TaskLed::~TaskLed()
{
}

void TaskLed::Run()
{
    static int counter = 0;

    if (IsTimePass(500))
    {
        if ((counter % 2) == 0)
        {
            digitalWrite(LED_PIN, HIGH);

            if (m_print_alive)
                LOG << "Alive " << (counter / 2) << "\n";            
        }
        else
        {
            digitalWrite(LED_PIN, LOW);
        }            

        counter++;
    }
}

void TaskLed::SetEnablePringAlive(bool enable)
{
    if (m_print_alive != enable)
    {
        m_print_alive = enable;
        Registry::Instance()->SetKey(REGISRTY_NAME_PRINT_ALIVE.c_str(), m_print_alive);
    }
}

bool TaskLed::GetEnablePringAlive()
{
    return m_print_alive;
}