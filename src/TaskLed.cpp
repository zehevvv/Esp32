#include "TaskLed.hpp"
#include "hw.h"
#include "iostream"
#include "Logger.hpp"

using namespace std;

TaskLed::TaskLed()
{
    pinMode(LED_PIN, OUTPUT);
    m_start_waiting = 0;
    m_time_to_wait = 0;    
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
            LOG << "Alive " << (counter / 2) << "\n";            
        }
        else
        {
            digitalWrite(LED_PIN, LOW);
        }            

        counter++;
    }
}
