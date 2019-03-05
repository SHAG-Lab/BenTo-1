/*
  ==============================================================================

    ColorNodeViewUI.h
    Created: 14 Apr 2018 5:07:30pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "NodeViewUI.h"
#include "../ColorNode.h"


class ColorNodeViewUI :
	public NodeViewUI
{
public:
	ColorNodeViewUI(ColorNode * cn);
	virtual ~ColorNodeViewUI();

	ColorNode * cn;

	virtual void paint(Graphics &g) override;

};