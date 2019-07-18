/*
  ==============================================================================

    LighttoysFTProp.h
    Created: 30 Jan 2019 10:04:12am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../../../Prop.h"
#include "Common/Serial/SerialManager.h"
#include "Common/Serial/SerialDeviceParameter.h"

#include "FTPropStatus.h"


#define FT_SERIAL_CHECK_TIMERID 10

class LighttoysFTProp :
	public Prop,
	public SerialDevice::SerialDeviceListener,
	public SerialManager::SerialManagerListener
{
public:
	LighttoysFTProp(var params);
	~LighttoysFTProp();

	SerialDeviceParameter * deviceParam;
	IntParameter * numPaired;
	IntParameter * numConnected;
	BoolParameter * autoResolution;

	Trigger * addNewPairing;
	Trigger * addToGroup;
	Trigger * finishPairing;

	IntParameter * bakingShowID;

	String deviceID;
	String lastOpenedDeviceID; //for ghosting
	SerialDevice * device;

	OwnedArray<FTPropStatus> statusList;
	ControllableContainer propsStatus;

	

	bool slaveCheckList[32];

	virtual void sendColorsToPropInternal() override;
	virtual void uploadBakedData(BakeData bakedColors) override;

	virtual void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *c) override;

	virtual void setCurrentDevice(SerialDevice *port);
	
	// Inherited via SerialDeviceListener
	virtual void portOpened(SerialDevice *) override;
	virtual void portClosed(SerialDevice *) override;
	virtual void portRemoved(SerialDevice *) override;
	virtual void serialDataReceived(const var &data) override;

	// Inherited via SerialManagerListener
	virtual void portAdded(SerialDeviceInfo * info) override;
	virtual void portRemoved(SerialDeviceInfo * info) override;

	virtual void sendMessage(StringRef command, const int propMask = -1, int numArgs = 0, ...);

	int getPropMaskForId(int propID) const { return 1 << propID; }
	int getPropMaskForRange(int startID, int endID) const { int result = 0; for (int i = startID; i <= endID; i++) result += 1 << i; return result; }

	virtual void timerCallback(int timerID) override;

	static void autoDetectRemotes();

	String getTypeString() const override { return "Lighttoys FT"; }
	static LighttoysFTProp * create(var params) { return new LighttoysFTProp(params); }
};