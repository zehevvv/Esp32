#include "BluetoothTask.hpp"
#include "hw.h"
#include "iostream"
#include "Vibration.hpp"
#include "Logger.hpp"

using namespace std;

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

BluetoothTask::BluetoothTask() : 
    m_counter(0),
    m_last_command_counter(200)
{
    // Create the BLE Device
    BLEDevice::init("ISee2");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    // Create the BLE Service
    BLEService *pService = pServer->createService(BLUETOOTH_SERVICE_UUID);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
        BLUETOOTH_CHARACTERISTIC_UUID,
        /******* Enum Type NIMBLE_PROPERTY now *******     
                         BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  |
                        BLECharacteristic::PROPERTY_NOTIFY |
                        BLECharacteristic::PROPERTY_INDICATE
                        );
                    **********************************************/
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::NOTIFY |
            NIMBLE_PROPERTY::INDICATE);

    // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
    // Create a BLE Descriptor
    /***************************************************   
     NOTE: DO NOT create a 2902 descriptor. 
    it will be created automatically if notifications 
    or indications are enabled on a characteristic.

    pCharacteristic->addDescriptor(new BLE2902());
    ****************************************************/
    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLUETOOTH_SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    /** Note, this could be left out as that is the default value */
    pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter

    BLEDevice::startAdvertising();
    LOG << "Waiting a client connection to notify... \n";
}

BluetoothTask::~BluetoothTask()
{
}

void BluetoothTask::Run()
{
    // notify changed value
    if (deviceConnected)
    {
        ReadFromBle();
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        LOG << "start advertising \n";
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
void BluetoothTask::onConnect(BLEServer *pServer)
{
    LOG << "Bluetooth connected \n";
    deviceConnected = true;
};

void BluetoothTask::onDisconnect(BLEServer *pServer)
{
    LOG << "Bluetooth disconnected \n";
    deviceConnected = false;
}

/***************** New - Security handled here ********************
****** Note: these are the same return values as defaults ********/
uint32_t BluetoothTask::onPassKeyRequest()
{
    LOG << "Server PassKeyRequest \n";
    return 123456;
}

bool BluetoothTask::onConfirmPIN(uint32_t pass_key)
{
    LOG << "The passkey YES/NO number: " << pass_key << "\n";
    return true;
}

void BluetoothTask::onAuthenticationComplete(ble_gap_conn_desc desc)
{
    LOG << "Starting BLE work! \n";
}
/*******************************************************************/

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

void BluetoothTask::HandleCmd(int cmdId, byte *buff, uint8_t command_counter,int buffLength)
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
                LOG << "buff " << m_bufReceive[0] << " " << 
                m_bufReceive[1] << " " <<
                m_bufReceive[2] << " " <<
                m_bufReceive[3] << " " <<
                m_bufReceive[4] << " " <<
                m_bufReceive[5] << " " <<
                m_bufReceive[6] << " " <<
                "\n";
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
    if (pCharacteristic->getDataLength() > 0)
    {
        string income_buff = pCharacteristic->getValue();

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
    pCharacteristic->setValue(buff, buffLength);
}
