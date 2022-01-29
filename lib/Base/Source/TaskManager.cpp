#include "TaskManager.hpp"
#include "Logger.hpp"
#include "Esp.h"
#include "Network.hpp"
#include "OTA.hpp"

TaskManager::TaskManager() :
    m_task_list()
{
}

TaskManager::~TaskManager() 
{
}

void TaskManager::AddTask(ITask* task)
{
    m_task_list.push_front(task);
}

void TaskManager::start()
{
    LOG << "Here we go >>>>> \n";
    while (1)
    {
        for (ITask* task : m_task_list)
        {
            task->Run();
        }        
    }
}

void TaskManager::CriticalError(const char* error_msg)
{
    LOG << "//////////////////////////////" << "\n";
    LOG << "/////// CRITIVAL_ERROR ///////" << "\n";
    LOG << "//////////////////////////////" << "\n";
    LOG << error_msg << "\n";
    LOG << "//////////////////////////////" << "\n";

    uint64_t start_time = millis();
    while(millis() - start_time < 1000)
    {
        Network::Instance()->Run();
        Logger::Instance()->Run();
        OTA::Instance()->Run();
    }

    ESP.restart();
}

ITask::ITask()
{
    TaskManager::Instance()->AddTask(this);
}

bool ITask::IsTimePass(uint64_t time_to_wait)
{
    if (time_to_wait != m_time_to_wait)
    {
        m_start_waiting = millis();
        m_time_to_wait = time_to_wait;
    }
    
    if (millis() - m_start_waiting >= m_time_to_wait)
    {
        m_start_waiting = millis();
        return true;
    }
    else
    {
        return false;
    }
}