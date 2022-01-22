#pragma once
#include <Arduino.h>

template<class T>
class Singleton
{
friend T;
private:
    static T* m_instance;

public:
    static T* Instance()
    {
        if (m_instance == NULL)
            m_instance = new T();
        
        return m_instance;
    }
};

template<class T>
T* Singleton<T>::m_instance = NULL;
