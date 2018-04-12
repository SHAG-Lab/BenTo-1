/*
  ==============================================================================

    LightBlockLibraryUI.cpp
    Created: 10 Apr 2018 10:58:08pm
    Author:  Ben

  ==============================================================================
*/

#include "LightBlockModelLibraryUI.h"


LightBlockModelLibraryUI::LightBlockModelLibraryUI(const String &contentName, LightBlockModelLibrary * library) :
	ShapeShifterContentComponent(contentName),
	library(library),
	genericGroupUI(&library->genericBlocks),
	liveFeedGroupUI(&library->liveFeedBlocks),
	userModelManagerUI("User contributed",&library->userBlocks)
{
	addAndMakeVisible(&genericGroupUI);
	addAndMakeVisible(&liveFeedGroupUI);
	addAndMakeVisible(&userModelManagerUI);
	
}

LightBlockModelLibraryUI::~LightBlockModelLibraryUI()
{
}

void LightBlockModelLibraryUI::paint(Graphics & g)
{
}

void LightBlockModelLibraryUI::resized()
{
	Rectangle<int> r = getLocalBounds();

	if (genericGroupUI.getWidth() == 0) genericGroupUI.setBounds(r);
	genericGroupUI.setBounds(r.removeFromTop(genericGroupUI.getHeight()));
	r.removeFromTop(10);

	if (liveFeedGroupUI.getWidth() == 0) liveFeedGroupUI.setBounds(r);
	liveFeedGroupUI.setBounds(r.removeFromTop(liveFeedGroupUI.getHeight()));
	r.removeFromTop(10);
	userModelManagerUI.setBounds(r);
}