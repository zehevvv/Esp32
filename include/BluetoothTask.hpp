#pragma once

#include "TaskManager.hpp"
#include <NimBLEDevice.h>

class BluetoothTask : public Task<BluetoothTask>, public BLEServerCallbacks,  public BLECharacteristicCallbacks
{
friend Singleton<BluetoothTask>;
protected:
    BluetoothTask();
    ~BluetoothTask();
    void Run();    

    // Callbacks of the bluetooth.
    void onWrite(BLECharacteristic *pCharacteristic);
    void onConnect(BLEServer *pServer);
    void onDisconnect(BLEServer *pServer);

private:
    void ReadFromBle();
    void Parse(byte newByte);
    void ReverseOne();
    void HandleCmd(int cmdId, byte *buff, int buffLength);
    void HandleGetBatteryLevelCmd();
    void HandleSetLevelVibrationCmd(byte* buff, int buffLength);
    void HandleGetLevelVibrationCmd();
    void WriteToBLE(const char* value);
    void WriteToBLE(byte* buff, int buffLength);

    byte m_bufReceive[10];
    uint8_t m_counter;    
    BLEServer *m_server = NULL;
    BLECharacteristic *m_characteristic;
    bool m_device_connected = false;
    BLEService *m_service;
    bool m_is_get_new_data = false;

    static const int STATUS_INTERVAL = 10000;
};
