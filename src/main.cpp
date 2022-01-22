#include "RangeSensor.hpp"
#include "Vibration.hpp"
#include "TaskLed.hpp"
#include "hw.h"
#include "TaskManager.hpp"
// #include "BluetoothTask.hpp"
#include "Registry.hpp"
#include "OTA.hpp"
#include "Logger.hpp"

void setup()
{
    Serial.begin(115200);

	// wait until serial port opens for native USB devices
	while (!Serial)
	{
		delay(1);
	}

	Registry::Instance();
	TaskLed::Instance();
	RangeSensor::Instance();
	Vibration::Instance();
	// BluetoothTask::Instance();
	OTA::Instance();

}

void loop()
{
	TaskManager::Instance()->start();
}