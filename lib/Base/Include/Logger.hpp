#pragma once

#include <inttypes.h>
#include "hw.h"
#include "TaskManager.hpp"
#include "WiFiUdp.h"

#define LOG	(*Logger::Instance())

class Logger : public Task<Logger>
{
friend Singleton<Logger>;
friend TaskManager;
public:
	Logger &operator<<(int buff);
	Logger &operator<<(const char* buff);
	Logger &operator<<(float buff);
	Logger &operator<<(double buff);
	Logger &operator<<(uint16_t buff);
	Logger &operator<<(uint32_t buff);
	Logger &operator<<(uint64_t buff);

protected:
	Logger();
	~Logger();
	void Run();
	void SaveToOfflineBuf();
	void PrintOfflineBuf();

	char* m_buf;
	uint16_t m_counter;
	char* m_buf_offline;
	uint16_t m_counter_offline;
	bool m_is_log_missed;
	WiFiUDP m_wifi;

	static const uint16_t m_buff_size = LOGGER_BUFFER_SIZE;
	static const uint16_t m_offline_buff_size = LOGGER_OFFLINE_BUFFER_SIZE;
};
