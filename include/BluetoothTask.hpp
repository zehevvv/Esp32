#pragma once

#include "TaskManager.hpp"
#include <NimBLEDevice.h>

class BluetoothTask : public Task<BluetoothTask>, public BLEServerCallbacks
{
friend Singleton<BluetoothTask>;
protected:
    BluetoothTask();
    ~BluetoothTask();
    void Run();    

    // Callbacks of the bluetooth.
    void onConnect(BLEServer *pServer);
    void onDisconnect(BLEServer *pServer);
    uint32_t onPassKeyRequest();
    bool onConfirmPIN(uint32_t pass_key);
    void onAuthenticationComplete(ble_gap_conn_desc desc);

private:
    void ReadFromBle();
    void Parse(byte newByte);
    void ReverseOne();
    void HandleCmd(int cmdId, byte *buff, uint8_t command_counter,int buffLength);
    void HandleGetBatteryLevelCmd();
    void HandleSetLevelVibrationCmd(byte* buff, int buffLength);
    void HandleGetLevelVibrationCmd();
    void WriteToBLE(const char* value);
    void WriteToBLE(byte* buff, int buffLength);

    byte m_bufReceive[10];
    uint8_t m_counter;
    int m_last_command_counter;
};
