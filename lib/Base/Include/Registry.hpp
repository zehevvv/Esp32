#pragma once

#include "Singleton.hpp"
#include "Preferences.h"
#include "string"
#include "iostream"
#include "Logger.hpp"

using namespace std;

class Registry : public Singleton<Registry>
{
friend Singleton<Registry>;
protected:
    Registry();
    ~Registry();

    static const string REGISTRY_NAME;

    Preferences m_preferences;

public:
    template<typename T>
    T GetKey(const char* key, T default_value);
    
    template<typename T>
    void SetKey(const char* key, T& value);
};


template<typename T>
T Registry::GetKey(const char* key, T default_value)
{
    if (std::is_same<T, uint32_t>::value || std::is_same<T, uint8_t>::value) 
    {
        return m_preferences.getUInt(key, default_value);
    }
    else
    {
        LOG << "Error: The type not supported \n";
        return default_value;
    }
}

template<typename T>
void Registry::SetKey(const char* key, T& value)
{
    if (std::is_same<T, uint32_t>::value || std::is_same<T, uint8_t>::value) 
    {
        m_preferences.putUInt(key, value);
    }
    else
    {
        LOG << "Error: The type not supported \n";
    }
}