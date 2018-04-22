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
	SequenceLayer(s,"Block Layer")
{
}

LightBlockLayer::~LightBlockLayer()
{
}


Array<Colour> LightBlockLayer::getColors(int id, int resolution, float time, var params)
{
	Array<LightBlockClip *> clips = blockClipManager.getClipsAtTime(sequence->currentTime->floatValue());

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
		clipColors.add(c->getColors(id, resolution, time-c->startTime->floatValue(),params));
	}

	for (int i = 0; i < resolution; i++)
	{
		uint8_t r = 0, g = 0, b = 0;

		for (int j = 0; j < clipColors.size(); j++)
		{
			if (i >= clipColors[j].size()) continue;

			r = jmin(r + clipColors[j][i].getRed(), 255);
			
			g = jmin(g + clipColors[j][i].getGreen(), 255);

			b = jmin(b + clipColors[j][i].getBlue(), 255);
		}

		result.set(i, (Colour(r, g, b)));
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
	return data;
}

void LightBlockLayer::loadJSONDataInternal(var data)
{
	SequenceLayer::loadJSONDataInternal(data);
	blockClipManager.loadJSONData(data.getProperty("blocks", var()));
}
