/*
  ==============================================================================

    FlowPoiProp.cpp
    Created: 10 Apr 2018 10:31:01pm
    Author:  Ben

  ==============================================================================
*/

#include "FlowPoiProp.h"

FlowPoiProp::FlowPoiProp(var params) :
	FlowtoysProp(getTypeString(),params)
{
	resolution->setValue(1);
}

FlowPoiProp::~FlowPoiProp()
{
}
