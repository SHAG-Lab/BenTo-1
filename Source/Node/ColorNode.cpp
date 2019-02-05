/*
  ==============================================================================

	ColorNode.cpp
	Created: 14 Apr 2018 1:00:32pm
	Author:  Ben

  ==============================================================================
*/

#include "ColorNode.h"
#include "ui/ColorNodeViewUI.h"

ColorNode::ColorNode(const String & name, var params) : 
	Node(name, params)
{
	mainColorsSlot = addColorSlot(false, "Colors");
}

ColorNode::~ColorNode()
{
}

Array<Colour> ColorNode::getColors(Prop * p, double time, var params)
{
	//to override by child classes
	Array<Colour> result;
	result.resize(p->resolution->intValue());
	result.fill(Colours::black);
	return result;
}

NodeViewUI * ColorNode::createUI()
{
	return new ColorNodeViewUI(this);
}
