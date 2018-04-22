/*
  ==============================================================================

	LightBlockModel.cpp
	Created: 10 Apr 2018 7:12:39pm
	Author:  Ben

  ==============================================================================
*/

#include "LightBlockModel.h"
#include "../LightBlock.h"
#include "Prop/Prop.h"

LightBlockModel::LightBlockModel(const String &name, var params) :
	LightBlockColorProvider(name, false),
	presetManager(this),
	modelNotifier(5)
{
	itemDataType = "LightBlockModel";
	paramsContainer = new ControllableContainer("Parameters");
	paramsContainer->saveAndLoadName = false;
	addChildControllableContainer(paramsContainer);
	addChildControllableContainer(&presetManager);
}

LightBlockModel::~LightBlockModel()
{
}

void LightBlockModel::clear()
{
	presetManager.clear();

	Array<WeakReference<Parameter>> pList = getModelParameters();
	for (auto &p : pList) p->resetValue();
}

Array<WeakReference<Parameter>> LightBlockModel::getModelParameters()
{
	if (paramsContainer == nullptr) return Array<WeakReference<Parameter>>();
	return paramsContainer->getAllParameters();
}

Array<Colour> LightBlockModel::getColors(int id, int resolution, float time, var params)
{
	Array<Colour> result;
	result.resize(resolution);
	result.fill(Colours::black);
	return result;
}

void LightBlockModel::setCustomThumbnail(String path)
{
	customThumbnailPath = path;
	modelListeners.call(&ModelListener::customThumbnailChanged, this);
	modelNotifier.addMessage(new ModelEvent(ModelEvent::CUSTOM_THUMBNAIL_CHANGED, this));
}

var LightBlockModel::getJSONData()
{
	var data = BaseItem::getJSONData();
	if (paramsContainer != nullptr)
	{
		var cData = paramsContainer->getJSONData();
		if (cData.getDynamicObject()->getProperties().size() > 0) data.getDynamicObject()->setProperty("parameters", cData);
	}
	if (presetManager.items.size() > 0) data.getDynamicObject()->setProperty("presets", presetManager.getJSONData());
	return data;
}

void LightBlockModel::loadJSONDataInternal(var data)
{
	if (paramsContainer != nullptr) paramsContainer->loadJSONData(data.getProperty("parameters", var()));
	presetManager.loadJSONData(data.getProperty("presets", var()));
}
void LightBlockModel::onControllableFeedbackUpdateInternal(ControllableContainer * cc, Controllable * c)
{
	if (cc == paramsContainer) providerListeners.call(&ProviderListener::providerParameterValueUpdated, this, dynamic_cast<Parameter *>(c));
}

void LightBlockModel::childStructureChanged(ControllableContainer * cc)
{
	if (cc == paramsContainer) providerListeners.call(&ProviderListener::providerParametersChanged, this);
}
