#include "RangeSensor.hpp"
#include "Vibration.hpp"
#include "TaskManager.hpp"
#include "BluetoothTask.hpp"
#include "OTA.hpp"
#include "hw.h"

void setup()
{
    Serial.begin(SERIAL_BAUDRATE);

	// wait until serial port opens for native USB devices
	while (!Serial)
	{
		delay(1);
	}

	if (OTA::Instance()->IsSystemStable())
	{
		RangeSensor::Instance();
		Vibration::Instance();	
		BluetoothTask::Instance();
	}
}

void loop()
{
	TaskManager::Instance()->start();
}