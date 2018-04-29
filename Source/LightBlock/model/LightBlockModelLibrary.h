/*
  ==============================================================================

    LightBlockModelLibrary.h
    Created: 10 Apr 2018 10:57:18pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "UserLightBlockModelManager.h"
#include "LightBlockModelGroup.h"

class LightBlockModelLibrary :
	public ControllableContainer
{
public:
	juce_DeclareSingleton(LightBlockModelLibrary, true)
	LightBlockModelLibrary();
	~LightBlockModelLibrary();
	


	LightBlockModelGroup genericBlocks;
	LightBlockModelGroup liveFeedBlocks;
	UserLightBlockModelManager pictureBlocks;	
	UserLightBlockModelManager nodeBlocks;
	UserLightBlockModelManager scriptBlocks;
	UserLightBlockModelManager timelineBlocks;


	//Generic / Patterns
	ScopedPointer<LightBlockModel> solidColorBlock;
	ScopedPointer<LightBlockModel> rainbowBlock;
	ScopedPointer<LightBlockModel> strobeBlock;
	ScopedPointer<LightBlockModel> noiseBlock;

	//Live Feed
	ScopedPointer<LightBlockModel> videoBlock;
	ScopedPointer<LightBlockModel> dmxBlock;

	void clear() override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	Array<LightBlockModel *> getAllModels(bool includeUserModels = true);
	LightBlockModel * getModelWithName(const String &modelName);

	static LightBlockColorProvider * showProvidersAndGet();
	static LightBlockColorProvider * showAllModelsAndGet(bool includePresets);
	static Array<LightBlockColorProvider *> fillProvidersMenu(PopupMenu &menu, bool includePresets, int startIndex = 1);
	static Array<LightBlockColorProvider *> fillUserLightBlockManagerMenu(UserLightBlockModelManager * manager, PopupMenu &menu, bool includePresets, int startIndex);

};