#include "BatteryManager.h"

const String BatteryEvent::eventNames[BatteryEvent::TYPES_MAX]{"update", "criticalLevel", "charging", "discharging"};

BatteryManager::BatteryManager() : Component("battery"),
                                   voltage(4.1f),
                                   rawValue(0),
                                   value(1),
                                   timeSinceLastBatterySent(0)
{
}

void BatteryManager::init()
{

#if HAS_BATTERY_SENSING
    pinMode(BATTERY_PIN, INPUT);
    analogSetPinAttenuation(BATTERY_PIN, ADC_0db);
#endif
}

void BatteryManager::update()
{
#if HAS_BATTERY_SENSING
    long curTime = millis();
    if(curTime > timeSinceLastBatterySent + batterySendTime)
    {
        rawValue = analogRead(BATTERY_PIN);
        value = (rawValue - minVal)*1.0f / (maxVal-minVal);
        voltage = (rawValue * 10 / 4.0f) / 1000;
        timeSinceLastBatterySent = curTime;

        sendEvent(BatteryEvent(BatteryEvent::Update));
    }
#endif
}
