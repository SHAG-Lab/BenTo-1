#include "SensorManager.h"

SensorManager::SensorManager() : Component("sensors")
{
}

void SensorManager::init(bool initIMU)
{
    batteryManager.init();
    batteryManager.addListener(std::bind(&SensorManager::batteryEvent, this, std::placeholders::_1));

    btManager.init();
    btManager.addListener(std::bind(&SensorManager::buttonEvent, this, std::placeholders::_1));

    if(initIMU) imuManager.init();
    imuManager.addListener(std::bind(&SensorManager::imuEvent, this, std::placeholders::_1));
}

void SensorManager::update()
{
    batteryManager.update();
    btManager.update();
    imuManager.update();
}

void SensorManager::batteryEvent(const BatteryEvent &e)
{
    var data[4];
    data[0].type = 'i';
    data[0].value.i = e.type;
    data[1].type = 'i';
    data[1].value.i = batteryManager.rawValue;
    data[2].type = 'f';
    data[2].value.f = batteryManager.value;
    data[3].type = 'f';
    data[3].value.f = batteryManager.voltage;
    sendEvent(SensorEvent(SensorEvent::BatteryUpdate, batteryManager.name, data, 4));
}

void SensorManager::buttonEvent(const ButtonEvent &e)
{
    int numBTData = e.type == ButtonEvent::MultiPress ? 1 : 0;
    var *data = (var *)malloc((numBTData + 1) * sizeof(var));
    data[0].value.i = e.type;
    data[0].type = 'i';

    if (e.type == ButtonEvent::MultiPress)
    {
        data[1].value.i = e.count;
        data[1].type = 'i';
    }
    
    sendEvent(SensorEvent(SensorEvent::ButtonUpdate, btManager.name, data, numBTData + 1));
}

void SensorManager::imuEvent(const IMUEvent &e)
{
    var data[4];
    data[0].type = 'i';
    data[0].value.i = e.type;

    for(int i=0;i<4;i++)
    {
        data[i+1].type = 'f';
        data[i+1].value.f = imuManager.orientation[i];
    }

    if(e.type == IMUEvent::OrientationUpdate) sendEvent(SensorEvent(SensorEvent::OrientationUpdate, imuManager.name, data, 5));
}