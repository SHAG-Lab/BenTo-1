/*
  ==============================================================================

    TimeTrigger.h
    Created: 20 Nov 2016 3:18:20pm
    Author:  Ben Kuper

  ==============================================================================
*/

#ifndef TIMETRIGGER_H_INCLUDED
#define TIMETRIGGER_H_INCLUDED

#include "JuceHeader.h"

class TimeTrigger :
	public BaseItem
{
public:
	TimeTrigger(float time = 0, float flagYPos = 0, const String &name = "Trigger");
	virtual ~TimeTrigger();

	FloatParameter * time;
	BoolParameter * isTriggered;
	
	BoolParameter * isLocked;

	//ui
	FloatParameter * flagY;

	void onContainerParameterChangedInternal(Parameter * p) override;

	void trigger();
	virtual void triggerInternal() {}
};



#endif  // TIMETRIGGER_H_INCLUDED
