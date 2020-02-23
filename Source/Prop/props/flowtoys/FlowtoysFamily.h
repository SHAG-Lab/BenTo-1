/*
  ==============================================================================

    FlowtoysFamily.h
    Created: 5 Feb 2019 11:21:43am
    Author:  jonglissimo

  ==============================================================================
*/

#pragma once

#include "../../PropFamily.h"
#include "Common/Serial/SerialManager.h"

class FlowtoysFamily :
	public PropFamily,
	public SerialManager::SerialManagerListener,
	public SerialDevice::SerialDeviceListener,
	public MultiTimer
{
public:
	FlowtoysFamily();
	~FlowtoysFamily();

	ControllableContainer clubCC;
	FloatParameter * irLevel;

	BoolParameter* autoAddSerialDevices;
	IntParameter * targetPropFPS;

	Array<SerialDevice *> pendingDevices;

	void onContainerParameterChanged(Parameter * p) override;
	void onControllableFeedbackUpdate(ControllableContainer * cc, Controllable *c) override;

	void portAdded(SerialDeviceInfo* d) override;
	void portRemoved(SerialDeviceInfo * d) override;
	
	void checkSerialDevices();
	void checkDeviceHardwareID(SerialDeviceInfo* d);
	Prop * addPropForHardwareID(SerialDevice *d, String firmware, String type);

	void serialDataReceived(SerialDevice* d, const var& data) override;
	void timerCallback(int timerID) override;
};