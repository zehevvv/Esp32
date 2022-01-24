#include "BluetoothTask.hpp"
#include "hw.h"
#include "iostream"
#include "Logger.hpp"
#include "CommandManager.hpp"

using namespace std;

BluetoothTask::BluetoothTask()    
{
    // Create the BLE Device
    BLEDevice::init(BLUETOOTH_APP_NAME);

    // Create the BLE Server
    m_server = BLEDevice::createServer();
    m_server->setCallbacks(this);

    // Create the BLE Service
    m_service = m_server->createService(BLUETOOTH_SERVICE_UUID);

    // Create a BLE Characteristic
    m_characteristic = m_service->createCharacteristic(
        BLUETOOTH_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ);

    m_characteristic->setCallbacks(this);

    // Start the service
    m_service->start();

    // Start advertising
    m_server->getAdvertising()->start();
}

BluetoothTask::~BluetoothTask()
{
}

void BluetoothTask::Run()
{
    ReadFromBle();

    if (m_device_connected)
    {
        if (IsTimePass(STATUS_INTERVAL))
        {
            CommandManager::Instance()->SendStatus();
        }
    }
}

void BluetoothTask::onConnect(BLEServer *m_server)
{
    m_device_connected = true;
};

void BluetoothTask::onDisconnect(BLEServer *m_server)
{
    m_device_connected = false;
}

void BluetoothTask::onWrite(BLECharacteristic *pCharacteristic)
{
    m_is_get_new_data = true;
}

void BluetoothTask::ReadFromBle()
{
    if (!m_is_get_new_data)
        return;
        
    m_is_get_new_data = false;
        
    if (m_characteristic->getDataLength() > 0)
    {
        string income_data = m_characteristic->getValue();
        CommandManager::Instance()->ReceiveData(income_data);
    }
}

void BluetoothTask::WriteToBLE(const char *value)
{
    WriteToBLE((byte *)value, strlen(value));
}

void BluetoothTask::WriteToBLE(byte *buff, int buffLength)
{
    // LOG << "Writing :";

    // for (int i = 0; i < buffLength; i++)
    // {
    //     LOG << (int)buff[i] << ", ";
    // }
    // LOG << "\n";
    m_characteristic->setValue(buff, buffLength);
    m_characteristic->notify();
}
