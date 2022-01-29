#include <OTA.hpp>
#include <ArduinoOTA.h>
#include "iostream"
#include "Network.hpp"
#include "Logger.hpp"
#include "LedTask.hpp"

using namespace std;

static RTC_NOINIT_ATTR int s_reset_counter;
static RTC_NOINIT_ATTR int s_reset_magic;

OTA::OTA()
{
    // Connected to WIFI
    Network::Instance();

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    // ArduinoOTA.setHostname("myesp32");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA    
    .onEnd([]() 
    {
        s_reset_counter = 0;
    });

    ArduinoOTA.begin();    
}

OTA::~OTA()
{
}

void OTA::Run()
{
    if (Network::Instance()->IsConnected())
        ArduinoOTA.handle();
}

bool OTA::IsSystemStable()
{	
	if (s_reset_magic != RESET_MAGIC)
	{
		s_reset_counter = 0;
		s_reset_magic = RESET_MAGIC;
		LOG << "magic reset was not set \n";
		return true;
	}
	else
	{
        s_reset_counter++;
		LOG << "magic reset was set, counter " << s_reset_counter <<  "\n";		
		if (s_reset_counter > MAX_ALLOWED_RESET)
		{
			LOG << "System not stable! \n";
            LedTask::Instance()->SetUnstableSystemBlink();
			return false;
		}
		else
		{
			return true;
		}
	}
}

