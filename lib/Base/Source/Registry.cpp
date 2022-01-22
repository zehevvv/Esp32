#include "Registry.hpp"
#include <type_traits>
#include "iostream"

using namespace std;

const string Registry::REGISTRY_NAME = "ISee2_registry";

Registry::Registry(/* args */)
{
    m_preferences.begin("REGISTRY_NAME", false);

}

Registry::~Registry()
{    
}

