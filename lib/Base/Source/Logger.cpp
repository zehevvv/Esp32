#include "Logger.hpp"
#include <iostream>
#include <inttypes.h>
#include <stdio.h>
#include <Arduino.h>
#include "Network.hpp"

using namespace std;

Logger::Logger()
{
    m_buf = new char[m_buff_size];
    m_counter = 0;
    m_buf_offline = new char[m_offline_buff_size];
    m_counter_offline = 0;
    m_is_log_missed = false;

    // Connect WIFI
    Network::Instance();

    IPAddress ip;
    ip.fromString(LOGGER_BROADCAST_IP);    
    m_wifi.beginPacket(ip, LOGGER_BROADCAST_PORT);
}

Logger::~Logger()
{
    delete[] m_buf;
    delete[] m_buf_offline;
}

Logger& Logger::operator<<(int buff)
{
    m_counter += snprintf(&m_buf[m_counter], m_buff_size - m_counter, "%d", buff);
    return *this;
}

Logger& Logger::operator<<(const char* buff)
{
    m_counter += snprintf(&m_buf[m_counter], m_buff_size - m_counter, "%s", buff);
    return *this;
}

Logger& Logger::operator<<(float buff)
{
    m_counter += snprintf(&m_buf[m_counter], m_buff_size - m_counter, "%f", buff);
    return *this;
}

Logger& Logger::operator<<(double buff)
{
    m_counter += snprintf(&m_buf[m_counter], m_buff_size - m_counter, "%lf", buff);
    return *this;
}

Logger& Logger::operator<<(uint16_t buff)
{
    m_counter += snprintf(&m_buf[m_counter], m_buff_size - m_counter, "%u", buff);
    return *this;
}

Logger& Logger::operator<<(uint32_t buff)
{
    m_counter += snprintf(&m_buf[m_counter], m_buff_size - m_counter, "%u", buff);
    return *this;
}

Logger& Logger::operator<<(uint64_t buff)
{
    m_counter += snprintf(&m_buf[m_counter], m_buff_size - m_counter, "%" PRIu64, buff);
    return *this;
}

void Logger::Run()
{
    if (Network::Instance()->IsConnected() && m_counter_offline > 0)
    {
        PrintOfflineBuf();
    }

    if (m_counter > 0)
    {        
        m_buf[m_counter] = 0;        
        Serial.write(m_buf, m_counter);

        if (Network::Instance()->IsConnected())
        {
            m_wifi.write((uint8_t*)m_buf, m_counter);                
            m_wifi.endPacket();
            m_wifi.beginPacket();
        }
        else
        {
            SaveToOfflineBuf();
        }

        m_counter = 0;
    }
}

void Logger::SaveToOfflineBuf()
{
    uint16_t size_to_copy = m_counter;
    if (m_counter > m_offline_buff_size - m_counter_offline)
    {
        size_to_copy = m_offline_buff_size - m_counter_offline;
        m_is_log_missed = true;
    }

    memcpy(&m_buf_offline[m_counter_offline], m_buf, size_to_copy);
    m_counter_offline += size_to_copy;
}

void Logger::PrintOfflineBuf()
{
    const char* MESSAGE_MISSED_LOGS = "Offline buffer missed some logs \n";
    m_wifi.write((uint8_t*)m_buf_offline, m_counter_offline);                
    if (m_is_log_missed)
        m_wifi.write((uint8_t*)MESSAGE_MISSED_LOGS, sizeof(MESSAGE_MISSED_LOGS));                

    m_wifi.endPacket();
    m_wifi.beginPacket();
    m_counter_offline = 0;
    m_is_log_missed = false;
}

