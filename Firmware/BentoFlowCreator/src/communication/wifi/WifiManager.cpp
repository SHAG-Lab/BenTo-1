
#include "WifiManager.h"

WifiManager::WifiManager() : 
    Component("wifi"),
    state(Off),
    lastConnectTime(0)
{
}

void WifiManager::init()
{
    NDBG("Init");
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
            if(WiFi.isConnected()) setState(Connected);
        }

        if(curTime > timeAtConnect + connectionTimeout)
        {
            setState(ConnectionError);
        }
    }
    break;

    default:
    break;
    }
}

void WifiManager::setState(ConnectionState s)
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

    if(state == Connected || state == Hotspot) WiFi.disconnect();
    
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.setSleep(false);
    WiFi.setTxPower(WIFI_POWER_19dBm);

    prefs.begin(name.c_str(), true);
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
    setState(Off);
}

void WifiManager::disable()
{
    WiFi.disconnect();
    setState(Disabled);
}

void WifiManager::saveWifiConfig(String ssid, String pass)
{
    prefs.begin(name.c_str());
    prefs.putString("ssid", ssid);
    prefs.getString("pass", pass);
    prefs.end();

    connect();
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

bool WifiManager::handleCommand(String command, var * data, int numData)
{
    if(checkCommand(command,"setCredentials",data, 2))
    {
        saveWifiConfig(data[0].stringValue(), data[1].stringValue());
        return true;
    }

    return false;
}