/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "JuceHeader.h"
#include "BentoEngine.h"
#include "MainComponent.h"



static OrganicApplication& getApp() { return *dynamic_cast<OrganicApplication*>(JUCEApplication::getInstance()); }
String getAppVersion() { return getApp().getApplicationVersion(); }
ApplicationProperties& getAppProperties() { return *getApp().appProperties; }

OpenGLContext * getOpenGLContext() { return &getApp().mainWindow->openGLContext; }

ApplicationCommandManager& getCommandManager() { return getApp().commandManager; }
OrganicApplication::MainWindow * getMainWindow() { return getApp().mainWindow; }


//==============================================================================
class BenToApplication  : public OrganicApplication
{
public:
    //==============================================================================
    BenToApplication() : OrganicApplication("BenTo") {}

    //==============================================================================
    void initialiseInternal (const String& commandLine) override
    {
		AppUpdater::getInstance()->setURLs(URL("http://benjamin.kuperberg.fr/bento/releases/update.json"), "http://benjamin.kuperberg.fr/bento/user/data/", "Bento");
		engine = new BentoEngine(appProperties,getAppVersion());
		mainComponent = new MainComponent();

		//ShapeShifterManager::getInstance()->setDefaultFileData(BinaryData::default_chalayout);
		ShapeShifterManager::getInstance()->setLayoutInformations("btlayout", "Bento/layouts");
    }
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (BenToApplication)
