#pragma once

#include "TaskManager.hpp"

class Network : public Task<Network>
{
friend Singleton<Network>;
friend TaskManager;
protected:
    Network(/* args */);
    ~Network();
    void Run();
    void UpdateConnectStatus();

    bool m_is_connected;
public:
    bool IsConnected();
};

