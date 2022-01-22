#include "BluetoothTask.hpp"
#include "hw.h"
#include "iostream"
#include "Vibration.hpp"
#include "Logger.hpp"

using namespace std;

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
// BLECharacteristic *pRxCharacteristic;
bool deviceConnected = false;
// bool oldDeviceConnected = false;
// uint8_t txValue = 0;
BLEService *pService;
bool m_is_get_new_data = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// #define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
// #define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
// #define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// /**  None of these are required as they will be handled by the library with defaults. **
//  **                       Remove as you see fit for your needs                        */
// class MyServerCallbacks : public BLEServerCallbacks
// {
//     void onConnect(BLEServer *pServer)
//     {
//         deviceConnected = true;
//     };

//     void onDisconnect(BLEServer *pServer)
//     {
//         deviceConnected = false;
//     }
// };

// class MyCallbacks : public BLECharacteristicCallbacks
// {
//     void onWrite(BLECharacteristic *pCharacteristic)
//     {
//         m_is_get_new_data = true;
//     }
// };

BluetoothTask::BluetoothTask() : m_counter(0),
                                 m_last_command_counter(200)
{

    // Create the BLE Device
    BLEDevice::init("ISee2");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    // Create the BLE Service
    pService = pServer->createService(BLUETOOTH_SERVICE_UUID);

    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic(
        BLUETOOTH_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE);

    pTxCharacteristic->setCallbacks(this);

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");
}

BluetoothTask::~BluetoothTask()
{
}

void BluetoothTask::Run()
{
    if (m_is_get_new_data)
    {
        m_is_get_new_data = false;
        ReadFromBle();
    }
}

void BluetoothTask::onConnect(BLEServer *pServer)
{
    deviceConnected = true;
};

void BluetoothTask::onDisconnect(BLEServer *pServer)
{
    deviceConnected = false;
}

void BluetoothTask::onWrite(BLECharacteristic *pCharacteristic)
{
    m_is_get_new_data = true;
}

void BluetoothTask::HandleSetLevelVibrationCmd(byte *buff, int buffLength)
{
    if (buff == NULL)
    {
        LOG << "Error: The buffer of vibration command is null \n";
        return;
    }
    if (buffLength != 1)
    {
        LOG << "Error: The length of buffer of vibration command is not 1, actual length - " << buffLength << "\n";
    }

    uint8_t level = buff[0];
    Vibration::Instance()->SetPowerLevel(level);
    LOG << "Set vibration level command with value - " << level << "\n";
}

void BluetoothTask::HandleGetLevelVibrationCmd()
{
    LOG << "Get command \"Get level vibration\" \n";
    byte cmd[6];
    cmd[0] = '<';
    cmd[1] = 0; // ID
    cmd[2] = 0; // Length 2
    cmd[3] = 1; // Length 1
    cmd[4] = Vibration::Instance()->GetPowerLevel();
    cmd[5] = '>';
    WriteToBLE(cmd, sizeof(cmd));
}

void BluetoothTask::HandleGetBatteryLevelCmd()
{
    byte cmd[6];
    cmd[0] = '<';
    cmd[1] = 1; // ID
    cmd[2] = 0; // Length 2
    cmd[3] = 1; // Length 1
    cmd[4] = 100;
    cmd[5] = '>';
    WriteToBLE(cmd, sizeof(cmd));
}

void BluetoothTask::HandleCmd(int cmdId, byte *buff, uint8_t command_counter, int buffLength)
{
    // Check if this command is new
    if (command_counter == m_last_command_counter)
        return;

    m_last_command_counter = command_counter;

    switch (cmdId)
    {
    case 0:
        HandleSetLevelVibrationCmd(buff, buffLength);
        break;
    case 1:
        HandleGetLevelVibrationCmd();
        break;
    case 2:
        HandleGetBatteryLevelCmd();
        break;
    default:
        LOG << "get unknown cmd " << cmdId << "\n";
    }
}

void BluetoothTask::ReverseOne()
{
    for (int i = 0; i < m_counter - 1; i++)
    {
        m_bufReceive[i] = m_bufReceive[i + 1];
    }

    m_counter--;
}

void BluetoothTask::Parse(byte newByte)
{
    m_bufReceive[m_counter] = newByte;
    m_counter++;

    while (m_counter != 0)
    {
        // Check if this not start of cmd
        if (m_bufReceive[0] != '<')
        {
            LOG << "Error: The letter " << m_bufReceive[0] << " is not < \n";
            ReverseOne();
        }
        // Check if not have enough bytes for minimum cmd
        else if (m_counter < 6)
        {
            return;
        }
        // Get enough data for start parse the command.
        else
        {
            int cmdLength = m_bufReceive[3] + (m_bufReceive[2] << 8);

            // Check if the command is more than buffer can hold
            if (cmdLength > sizeof(m_bufReceive) - 6)
            {
                LOG << "Error: Cmd length is " << cmdLength << "\n";
                ReverseOne();
            }
            // Check if not get all the bytes of the command
            else if (m_counter < cmdLength + 6)
            {
                return;
            }
            // Check if not get the '>' (end of the command)
            else if (m_bufReceive[cmdLength + 5] != '>')
            {
                LOG << "buff " << m_bufReceive[0] << " " << m_bufReceive[1] << " " << m_bufReceive[2] << " " << m_bufReceive[3] << " " << m_bufReceive[4] << " " << m_bufReceive[5] << " " << m_bufReceive[6] << " "
                    << "\n";
                LOG << "Error: The last letter is not '>', it is " << m_bufReceive[cmdLength + 5] << "\n";
                ReverseOne();
            }
            // Parse command success!!!
            else
            {
                LOG << "Get command - " << (int)m_bufReceive[1] << ", command size - " << cmdLength << "\n";
                HandleCmd((int)m_bufReceive[1], &m_bufReceive[5], m_bufReceive[4], cmdLength);

                // Assumming that the counter is point to last char of the command.
                m_counter = 0;
            }
        }
    }
}

void BluetoothTask::ReadFromBle()
{
    if (pTxCharacteristic->getDataLength() > 0)
    {
        string income_buff = pTxCharacteristic->getValue();

        for (size_t i = 0; i < income_buff.size(); i++)
        {
            Parse(income_buff[i]);
            LOG << income_buff[i] << "\n";
        }
    }
}

void BluetoothTask::WriteToBLE(const char *value)
{
    WriteToBLE((byte *)value, strlen(value));
}

void BluetoothTask::WriteToBLE(byte *buff, int buffLength)
{
    LOG << "Writing :";

    for (int i = 0; i < buffLength; i++)
    {
        LOG << (int)buff[i] << ", ";
    }
    LOG << "\n";
    pTxCharacteristic->setValue(buff, buffLength);
}
