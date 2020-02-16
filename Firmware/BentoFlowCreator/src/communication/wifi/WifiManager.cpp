
#include "WifiManager.h"

WifiManager::WifiManager() : 
    Component("wifi"),
    lastConnectTime(0)
{
}

void WifiManager::init()
{
    connect();
}

void WifiManager::update()
{
    switch (state)
    {
    case Connecting:
    {
        long curTime = millis();
        if (curTime > lastConnectTime + timeBetweenTries)
        {
            setState(Connected);
        }

        if(curTime > timeAtConnect + connectionTimeout)
        {
            setState(Disconnected);
        }
    }
    break;

    default:
    break;
    }
}

void WifiManager::setState(WifiState s)
{
    if (state == s)
        return;

    state = s;
    sendEvent();
}

void WifiManager::connect()
{
    lastConnectTime = millis();
    timeAtConnect = millis();

    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.setSleep(false);
    WiFi.setTxPower(WIFI_POWER_19dBm);

    prefs.begin("wifiConfig");
    String ssid = prefs.getString("ssid", "jonglissimo");
    String pass = prefs.getString("pass", "lightpainting");
    prefs.end();

    NDBG("Connecting to "+ssid+" : "+pass+"...");
    WiFi.begin(ssid.c_str(), pass.c_str());

    setState(Connecting);
}

void WifiManager::disconnect()
{
    WiFi.disconnect();
    setState(Disconnected);
}

void WifiManager::turnOff()
{
    WiFi.disconnect();
    setState(Off);
}


String WifiManager::getIP()
{
    if(state == Connected) return String(WiFi.localIP()[0]) +
              "." + String(WiFi.localIP()[1]) +
              "." + String(WiFi.localIP()[2]) +
              "." + String(WiFi.localIP()[3]);

    else if(state == Hotspot) return String(WiFi.softAPIP()[0]) + 
              "." + String( WiFi.softAPIP()[1]) + 
              "." + String( WiFi.softAPIP()[2]) + 
              "." + String( WiFi.softAPIP()[3]);

    return "[noip]";
}