#include <OTA.hpp>
#include <ArduinoOTA.h>
#include "iostream"
#include "Network.hpp"

using namespace std;

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
