/*
  ==============================================================================

    RGBComponent.cpp
    Created: 8 May 2020 3:09:02pm
    Author:  bkupe

  ==============================================================================
*/

#include "RGBComponent.h"

RGBPropComponent::RGBPropComponent(Prop* prop, var params) :
    PropComponent(prop, "RGB"),
    updateRate(params.getProperty("updateRate", 50)),
    resolution(params.getProperty("resolution",32))
{
    brightness = addFloatParameter("Brightness", "Brightness", 1, 0, 1);
}

RGBPropComponent::~RGBPropComponent()
{
}
