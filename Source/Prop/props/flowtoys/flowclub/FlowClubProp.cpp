/*
  ==============================================================================

    FlowClubProp.cpp
    Created: 10 Apr 2018 10:29:13pm
    Author:  Ben

  ==============================================================================
*/

#include "FlowClubProp.h"

FlowClubProp::FlowClubProp(var params) :
	FlowtoysProp(getTypeString(),params)
{
	resolution->setValue(32);
	button = sensorsCC.addBoolParameter("Button", "Is the button on the prop pressed ?", false);
	button->setControllableFeedbackOnly(true);
}

FlowClubProp::~FlowClubProp()
{
}
