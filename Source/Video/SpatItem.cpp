/*
  ==============================================================================

    SpatItem.cpp
    Created: 23 Apr 2018 9:43:20pm
    Author:  Ben

  ==============================================================================
*/

#include "SpatItem.h"

SpatItem::SpatItem() :
	BaseItem("Spat Item")
{

	id = addIntParameter("Target ID", "The id of the props to assign this", 0, 0, INT32_MAX);
	isDefault = addBoolParameter("Default", "If checked and no item with the requested id is found, will fall back to this one", false);
	shape = addEnumParameter("Shape", "The shape of the prop");
	shape->addOption("Club", Prop::Shape::CLUB)->addOption("Ball", Prop::Shape::BALL)->addOption("Poi", Prop::Shape::POI)->addOption("Hoop", Prop::Shape::HOOP);

	resolution = addIntParameter("Resolution", "Number of controllable colors in the prop", 32, 1, INT32_MAX);
	startPos = addPoint2DParameter("Start", "Start Position");
	endPos = addPoint2DParameter("End", "End Position");

	startPos->setPoint(.4f, .5f);
	endPos->setPoint(.6f, .5f);
}

SpatItem::~SpatItem()
{

}

void SpatItem::updatePoints()
{
	points.clear();
	Point<float> startPoint = startPos->getPoint();
	Point<float> endPoint = endPos->getPoint();

	int numPoints = resolution->intValue();

	switch (shape->getValueDataAsEnum<Prop::Shape>())
	{
	case Prop::Shape::CLUB:
		for (int i = 0; i < numPoints ; i++)
		{
			points.add(startPoint + (endPoint-startPoint) * (i * 1.0f / jmax(numPoints - 1, 1)));
		}
		break;

	case Prop::Shape::BALL:
		break;

	case Prop::Shape::POI:
		break;

	case Prop::Shape::HOOP:
		break;

	
	}

	colors.resize(points.size());
}

void SpatItem::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == startPos || p == endPos || p == shape) updatePoints();
}
