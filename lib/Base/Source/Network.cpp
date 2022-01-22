#include "Network.hpp"
#include "Logger.hpp"
#include "WiFi.h"
#include "hw.h"
#include "iostream"

using namespace std;

Network::Network() :
    m_is_connected(false)
{    
    WiFi.mode(WIFI_STA);
    WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
}

Network::~Network()
{
}

void Network::Run()
{
    UpdateConnectStatus();
}

bool Network::IsConnected()
{
    return m_is_connected; 
}

void Network::UpdateConnectStatus()
{
    bool connected = false;

    if (WiFi.isConnected() && 
        memcmp("0.0.0.0", WiFi.localIP().toString().c_str(), 7) != 0)
    {
        connected = true;
    }
    
    if (connected && !m_is_connected)
    {
        LOG << "Connected to network, IP = " << WiFi.localIP().toString().c_str() << "\n";    
        m_is_connected = true;
    }
    else if (!connected && m_is_connected)
    {
        LOG << "Disconnect from network \n";    
        m_is_connected = false;
    }
}