#pragma once

#include "wifi/WifiManager.h"
#include "serial/SerialManager.h"
#include "osc/OSCManager.h"

class CommunicationEvent
{
public:
    enum Type
    {
        MessageReceived,
        TYPES_MAX
    };
    const String typeNames[TYPES_MAX]{"MessageReceived"};

    CommunicationEvent(Type type, String source, String target, String command, var *data, int numData) : 
        type(type), source(source), target(target), command(command), data(data), numData(numData)
    {
    }

    Type type;
    String source;
    String target;
    String command;
    var *data;
    int numData;

    String toString() const
    {
        String s = "*" + typeNames[type] + "* source : " + source + ", target : " + target + ", command : "+command+", data : ";
        for (int i = 0; i < numData; i++)
            s += (i > 0 ? ", " : "") + data[i].stringValue()+ "("+data[i].type+")";
        
       return s;
    }
};

class CommunicationManager : public Component,
                             public EventBroadcaster<CommunicationEvent>
{
public:
    CommunicationManager();
    ~CommunicationManager() {}

    WifiManager wifiManager;
    SerialManager serialManager;
    OSCManager oscManager;

    void init();
    void update();

    void serialMessageEvent(const SerialEvent &e);
    void wifiConnectionEvent(const WifiManagerEvent &e);
    void oscMessageEvent(const OSCEvent &e);
};