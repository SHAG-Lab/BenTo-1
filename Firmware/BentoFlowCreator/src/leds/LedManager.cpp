#include "LedManager.h"

LedManager::LedManager() : Component("leds"),
                           sysLedMode(rgbManager.leds, LEDS_COUNT)
{
}

void LedManager::init()
{
    rgbManager.init();
    sysLedMode.init();

    setMode(System);
}

void LedManager::update()
{
    if (currentMode != nullptr)
    {
        rgbManager.clear();
        currentMode->update();
    }

    rgbManager.update();
}

void LedManager::setMode(Mode m)
{
    mode = m;
    switch (mode)
    {
    case Direct:
        currentMode = nullptr;
        rgbManager.clear();
        break;

    case System:
        currentMode = &sysLedMode;
        break;
    //case Stream: currentMode = &streamMode; break;
    //case Player: currentMode = &playerMode; break;
    default:
        break;
    }
}

void LedManager::shutdown(CRGB color)
{
    CRGB initLeds[LEDS_COUNT];
    memcpy(initLeds, rgbManager.leds, LEDS_COUNT * sizeof(CRGB));

    for (int i = 0; i < 255; i++)
    {
        for (int led = 0; led < LEDS_COUNT; led++)
            rgbManager.setLed(led, blend(initLeds[led], color, i));
        FastLED.delay(2);
    }

    FastLED.delay(50);

    for (float i = 1; i >= 0; i -= .01f)
    {
        rgbManager.fillRange(color, 0, i, true);
        rgbManager.update();
        FastLED.delay(3);
    }

    rgbManager.clear();
    rgbManager.update();

    delay(100);
}

bool LedManager::handleCommand(String command, var *data, int numData)
{
    if (checkCommand(command, "mode", numData, 1))
    {
        if (data[0].type == 's')
        {
            if (data[0].stringValue() == "direct")
                setMode(Direct);
            else if (data[0].stringValue() == "system")
                setMode(System);
            else if (data[0].stringValue() == "stream")
                setMode(Stream);
            else if (data[0].stringValue() == "player")
                setMode(Player);
        }
        else
        {
            setMode((Mode)data[0].intValue());
        }
    }
}
