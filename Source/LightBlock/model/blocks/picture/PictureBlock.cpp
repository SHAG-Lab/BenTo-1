/*
  ==============================================================================

	PictureBlock.cpp
	Created: 22 Apr 2018 8:30:42pm
	Author:  Ben

  ==============================================================================
*/

#include "PictureBlock.h"
#include "ui/PictureBlockUI.h"

PictureBlock::PictureBlock(var params) :
	LightBlockModel(getTypeString(), params)
{
	pictureFile = new FileParameter("Picture File", "The file, the picture, the thing", "");
	addParameter(pictureFile);

	speed = paramsContainer->addFloatParameter("Speed", "The speed", 1, 0, 50);
	offsetByID = paramsContainer->addFloatParameter("Offset by ID", "The offset", 0, 0, 1);
	brightness = paramsContainer->addFloatParameter("Brightness", "The brightness", 1, 0, 1);
	saturation = paramsContainer->addFloatParameter("Saturation", "The saturation", 1, 0, 3);
	hue = paramsContainer->addFloatParameter("Hue", "The hue", 0, -1, 1);
}

void PictureBlock::getColorsInternal(Array<Colour> * result, Prop * p, double time, int id, int resolution, var params)
{
	if (picture.getWidth() == 0) return;

	float bSpeed = getParamValue<float>(speed, params);
	float bOffsetByID = getParamValue<float>(offsetByID, params);
	float bBrightness = getParamValue<float>(brightness, params);
	float bSaturation = getParamValue<float>(saturation, params);
	float bHue = getParamValue<float>(hue, params);
	
	int numPixelsH = picture.getHeight();
	float txRel = fmodf(time * bSpeed + bOffsetByID * id, 1);
	int  tx = jmin<int>(txRel * picture.getWidth(), picture.getWidth() - 1);

	for (int i = 0; i < resolution; i++)
	{
		float ty = jmin(i*numPixelsH / resolution, numPixelsH - 1);
		
		float hue = 0, saturation = 0, brightness = 0;
		picture.getPixelAt(tx, ty).getHSB(hue, saturation, brightness);
		
		hue += bHue;
		saturation = jmin (1.0f, saturation * bSaturation);
		brightness = jmin (1.0f, brightness * bBrightness);
		
		result->set(i, Colour::fromHSV(hue,saturation,brightness, 1));
	}
}

void PictureBlock::onContainerParameterChangedInternal(Parameter * p)
{
	LightBlockModel::onContainerParameterChangedInternal(p);

	if (p == pictureFile)
	{
		File f = pictureFile->getFile();
		picture = ImageCache::getFromFile(f);
	}
}


LightBlockModelUI * PictureBlock::createUI()
{
	return new PictureBlockUI(this);
}

var PictureBlock::getJSONData()
{
	return LightBlockModel::getJSONData();
}
