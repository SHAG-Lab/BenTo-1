/*
  ==============================================================================

	LightBlockLayer.cpp
	Created: 17 Apr 2018 5:10:36pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockLayer.h"
#include "ui/LightBlockLayerPanel.h"
#include "ui/LightBlockLayerTimeline.h"

LightBlockLayer::LightBlockLayer(Sequence * s, var) :
	SequenceLayer(s, "Block Layer"),
	blockClipManager(this)
{
	defaultLayer = addBoolParameter("Default", "If checked, this layer will be the default layer when no layer has the requested prop id", false);
	previewID = addIntParameter("Preview ID", "ID to preview the content", 0, 0, 500);
	previewID->hideInEditor = true;

	addChildControllableContainer(&filterManager);
}

LightBlockLayer::~LightBlockLayer()
{

}

Array<Colour> LightBlockLayer::getColors(Prop * p, double time, var params)
{
	Array<LightBlockClip *> clips = blockClipManager.getClipsAtTime(time);

	int resolution = p->resolution->intValue();

	Array<Colour> result;
	result.resize(resolution);

	if (clips.size() == 0)
	{
		result.fill(Colours::black);
		return result;
	}

	Array<Array<Colour>> clipColors;

	for (auto &c : clips)
	{
		clipColors.add(c->getColors(p, time - c->startTime->floatValue(), params));
	}

	for (int i = 0; i < resolution; i++)
	{
		float r = 0, g = 0, b = 0;

		for (int j = 0; j < clipColors.size(); j++)
		{
			if (i >= clipColors[j].size()) continue;

			r += clipColors[j][i].getFloatRed();
			g += clipColors[j][i].getFloatGreen();
			b += clipColors[j][i].getFloatBlue();
		}

		result.set(i, (Colour::fromFloatRGBA(jmin(r, 1.f), jmin(g, 1.f), jmin(b, 1.f), 1)));
	}

	return result;
}

SequenceLayerPanel * LightBlockLayer::getPanel()
{
	return new LightBlockLayerPanel(this);
}

SequenceLayerTimeline * LightBlockLayer::getTimelineUI()
{
	return new LightBlockLayerTimeline(this);
}

var LightBlockLayer::getJSONData()
{
	var data = SequenceLayer::getJSONData();
	data.getDynamicObject()->setProperty("blocks", blockClipManager.getJSONData());
	data.getDynamicObject()->setProperty("filters", filterManager.getJSONData());
	return data;
}

void LightBlockLayer::loadJSONDataInternal(var data)
{
	SequenceLayer::loadJSONDataInternal(data);
	blockClipManager.loadJSONData(data.getProperty("blocks", var()));
	filterManager.loadJSONData(data.getProperty("filters", var()));
}
