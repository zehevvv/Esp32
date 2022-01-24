#include "CommandManager.hpp"
#include "Logger.hpp"
#include "Vibration.hpp"
#include "BluetoothTask.hpp"

CommandManager::CommandManager() : 
    m_counter(0)    
{
}

CommandManager::~CommandManager()
{
}

void CommandManager::HandleSetLevelVibrationCmd(byte *buff, int buffLength)
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

void CommandManager::HandleGetLevelVibrationCmd()
{
    // LOG << "Get command \"Get level vibration\" \n";
    byte cmd[6];
    cmd[0] = '<';
    cmd[1] = 0; // ID
    cmd[2] = 0; // Length 2
    cmd[3] = 1; // Length 1
    cmd[4] = Vibration::Instance()->GetPowerLevel();
    cmd[5] = '>';
    
    BluetoothTask::Instance()->WriteToBLE(cmd, sizeof(cmd));
}

void CommandManager::HandleGetBatteryLevelCmd()
{
    byte cmd[6];
    cmd[0] = '<';
    cmd[1] = 1; // ID
    cmd[2] = 0; // Length 2
    cmd[3] = 1; // Length 1
    cmd[4] = 100;
    cmd[5] = '>';
    
    BluetoothTask::Instance()->WriteToBLE(cmd, sizeof(cmd));
}

void CommandManager::HandleCmd(int cmdId, byte *buff, int buffLength)
{
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

void CommandManager::ReverseOne()
{
    for (int i = 0; i < m_counter - 1; i++)
    {
        m_bufReceive[i] = m_bufReceive[i + 1];
    }

    m_counter--;
}

void CommandManager::Parse(byte newByte)
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
        else if (m_counter < 5)
        {
            return;
        }
        // Get enough data for start parse the command.
        else
        {
            int cmdLength = m_bufReceive[3] + (m_bufReceive[2] << 8);

            // Check if the command is more than buffer can hold
            if (cmdLength > sizeof(m_bufReceive) - 5)
            {
                LOG << "Error: Cmd length is " << cmdLength << "\n";
                ReverseOne();
            }
            // Check if not get all the bytes of the command
            else if (m_counter < cmdLength + 5)
            {
                return;
            }
            // Check if not get the '>' (end of the command)
            else if (m_bufReceive[cmdLength + 4] != '>')
            {
                                
                LOG << "Error: The last letter is not '>', it is " << m_bufReceive[cmdLength + 4] << "\n";
                ReverseOne();
            }
            // Parse command success!!!
            else
            {
                LOG << "Get command - " << (int)m_bufReceive[1] << ", command size - " << cmdLength << "\n";
                HandleCmd((int)m_bufReceive[1], &m_bufReceive[4], cmdLength);

                // Assumming that the counter is point to last char of the command.
                m_counter = 0;
            }
        }
    }
}

void CommandManager::ReceiveData(string& data)
{        
    for (size_t i = 0; i < data.size(); i++)
    {
        Parse(data[i]);
        // LOG << data[i] << "\n";
    }
}

void CommandManager::SendStatus()
{
    HandleGetBatteryLevelCmd();
    HandleGetLevelVibrationCmd();
}
    