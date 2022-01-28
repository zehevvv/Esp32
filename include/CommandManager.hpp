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
    void HandleSetConfigCmd(byte* buff, int buffLength);
    void HandleGetConfigCmd();

    byte m_bufReceive[20];
    uint8_t m_counter;    

public:
    void ReceiveData(string& data);
    void SendStatus();
};

#pragma pack(1)
typedef struct SET_CONFIG_CMD
{
    uint16_t min_range;
    uint16_t max_range;
    uint16_t min_cycle;
    uint16_t max_cycle;
    uint8_t min_vibration_level;
    uint8_t max_vibration_level;
    uint8_t sensor_mode;
    uint8_t enable_vibration;
    uint8_t print_range_unfiltered;
    uint8_t print_cycle_config;
    uint8_t print_alive;  
} SET_CONFIG_CMD;
#pragma pack()

