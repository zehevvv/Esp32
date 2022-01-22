#include "BluetoothTask.hpp"
#include "hw.h"
#include "iostream"
#include "Vibration.hpp"
#include "Logger.hpp"

using namespace std;

BluetoothTask::BluetoothTask() : 
    m_counter(0),
    m_last_command_counter(200)
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
            HandleGetBatteryLevelCmd();
            HandleGetLevelVibrationCmd();
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
    if (!m_is_get_new_data)
        return;
        
    m_is_get_new_data = false;
        
    if (m_characteristic->getDataLength() > 0)
    {
        string income_buff = m_characteristic->getValue();

        for (size_t i = 0; i < income_buff.size(); i++)
        {
            Parse(income_buff[i]);
            // LOG << income_buff[i] << "\n";
        }
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
