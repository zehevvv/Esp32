#pragma once

#include "Singleton.hpp"
#include "string.h"

using namespace std;

class CommandManager : public Singleton<CommandManager>
{
friend Singleton<CommandManager>;
protected:
    CommandManager();
    ~CommandManager();

    void Parse(byte newByte);
    void ReverseOne();
    void HandleCmd(int cmdId, byte *buff, int buffLength);
    void HandleGetBatteryLevelCmd();
    void HandleSetLevelVibrationCmd(byte* buff, int buffLength);
    void HandleGetLevelVibrationCmd();

    byte m_bufReceive[10];
    uint8_t m_counter;    

public:
    void ReceiveData(string& data);
    void SendStatus();
};


