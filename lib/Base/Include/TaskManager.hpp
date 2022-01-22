#pragma once

#include "Singleton.hpp"
#include "list"

using namespace std;

class ITask;

class TaskManager : public Singleton<TaskManager>
{
friend Singleton;
private:
    TaskManager(/* args */);
    ~TaskManager();

    list<ITask*> m_task_list;

public:
    void AddTask(ITask* task);
    void start();
    void CriticalError(char* error_msg);
};


class ITask 
{
friend TaskManager;
protected:
    ITask();
    virtual void Run() = 0;
    bool IsTimePass(uint64_t time_to_wait);

    uint64_t m_start_waiting;
    uint64_t m_time_to_wait;
};


template <typename T>
class Task : public ITask, public Singleton<T>
{
friend Singleton<T>;
protected:
    virtual void Run() = 0;
};
