/*
  ==============================================================================

    FlowClubProp.h
    Created: 10 Apr 2018 10:29:13pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../FlowtoysProp.h"

class FlowClubProp :
	public FlowtoysProp
{
public:
	FlowClubProp(var params);
	~FlowClubProp();

	static const int vidFilter = 0x10c4;
	static const int pidFilter = 0xea60;

	virtual void clearItem() override;

	ControllableContainer batteryCC;
	FloatParameter* batteryLevel;
	FloatParameter* batteryVoltage;

	ControllableContainer buttonCC;
	BoolParameter * button;
	Trigger* shortPress;
	Trigger* longPress;
	IntParameter* multiPress;

	EnablingControllableContainer imuCC;
	IntParameter* imuUpdateRate;
	Point3DParameter* orientation;

	ControllableContainer irCC;
	FloatParameter * irLevel;

	void onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable *c) override;

	virtual void serialDataReceived(SerialDevice* d, const var& data) override;

	String getTypeString() const override { return  FlowClubProp::getTypeStringStatic(); }
	static const String getTypeStringStatic() { return "Flowtoys Creator Club"; }

	static FlowClubProp * create(var params) { return new FlowClubProp(params); } 

};