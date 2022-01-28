#include "CommandManager.hpp"
#include "Logger.hpp"
#include "Vibration.hpp"
#include "BluetoothTask.hpp"
#include "RangeSensor.hpp"
#include "TaskLed.hpp"

CommandManager::CommandManager() : 
    m_counter(0)    
{
}

CommandManager::~CommandManager()
{
}

void CommandManager::HandleSetConfigCmd(byte *buff, int buffLength)
{
    if (buff == NULL)
    {
        LOG << "Error: The buffer of vibration command is null \n";
        return;
    }
    if (buffLength != sizeof(SET_CONFIG_CMD))
    {
        LOG << "Error: The length of buffer of vibration command is not " << sizeof(SET_CONFIG_CMD) << " , actual length - " << buffLength << "\n";
    }

    SET_CONFIG_CMD* cmd = (SET_CONFIG_CMD*) buff;
    Vibration::Instance()->SetMinPower(cmd->min_vibration_level);
    Vibration::Instance()->SetMaxPower(cmd->max_vibration_level);
    RangeSensor::Instance()->SetEnablePringRange(cmd->print_range_unfiltered);
    TaskLed::Instance()->SetEnablePringAlive(cmd->print_alive);
    Vibration::Instance()->SetEnablePrintCycleConfig(cmd->print_cycle_config);
    RangeSensor::Instance()->SetMode((RANGE_SENSOR_MODE)cmd->sensor_mode);
    RangeSensor::Instance()->SetMinRange(cmd->min_range);
    RangeSensor::Instance()->SetMaxRange(cmd->max_range);
    Vibration::Instance()->SetMinCycle(cmd->min_cycle);
    Vibration::Instance()->SetMaxCycle(cmd->max_cycle);
    
    LOG << "Set min vibration - " << cmd->min_vibration_level << "\n"
        << "Set max vibration - " << cmd->max_vibration_level << "\n"
        << "Set print enable - " << cmd->print_range_unfiltered << "\n"
        << "Set print alive - " << cmd->print_alive << "\n"
        << "Set print cycle config - " << cmd->print_cycle_config << "\n"
        << "Set sensor mode - " << cmd->sensor_mode << "\n"
        << "Set min range - " << cmd->min_range << "\n"
        << "Set max range - " << cmd->max_range << "\n"
        << "Set min cycle - " << cmd->min_cycle << "\n"
        << "Set max cycle - " << cmd->max_cycle << "\n";
}

void CommandManager::HandleGetConfigCmd()
{
    // LOG << "Get command \"Get level vibration\" \n";
    byte cmd[5 + sizeof(SET_CONFIG_CMD)];
    cmd[0] = '<';
    cmd[1] = 0; // ID
    cmd[2] = 0; // Length 2
    cmd[3] =  sizeof(SET_CONFIG_CMD); // Length 1
    cmd[sizeof(SET_CONFIG_CMD) + 4] = '>';

    SET_CONFIG_CMD* config = (SET_CONFIG_CMD*)&(cmd[4]);
    config->min_vibration_level = Vibration::Instance()->GetMinPower();
    config->max_vibration_level = Vibration::Instance()->GetMaxPower();
    config->print_range_unfiltered = (uint8_t)RangeSensor::Instance()->GetEnablePringRange();
    config->print_alive = (uint8_t)TaskLed::Instance()->GetEnablePringAlive();
    config->print_cycle_config = (uint8_t)Vibration::Instance()->GetEnablePrintCycleConfig();
    config->sensor_mode = (uint8_t)RangeSensor::Instance()->GetMode();
    config->min_range = RangeSensor::Instance()->GetMinRange();
    config->max_range = RangeSensor::Instance()->GetMaxRange();
    config->min_cycle = Vibration::Instance()->GetMinCycle();
    config->max_cycle = Vibration::Instance()->GetMaxCycle();
    
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
        HandleSetConfigCmd(buff, buffLength);
        break;
    case 1:
        HandleGetConfigCmd();
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
    HandleGetConfigCmd();
}
    