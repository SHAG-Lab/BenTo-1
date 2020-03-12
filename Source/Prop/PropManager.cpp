/*
  ==============================================================================

	PropManager.cpp
	Created: 10 Apr 2018 6:59:29pm
	Author:  Ben

  ==============================================================================
*/

#include "PropManager.h"
juce_ImplementSingleton(PropManager)

#include "props/flowtoys/FlowtoysFamily.h"
#include "props/flowtoys/flowclub/FlowClubProp.h"

#include "props/garnav/GarnavFamily.h"
#include "props/garnav/SmarballProp.h"

#include "props/lighttoys/LighttoysFamily.h"
#include "props/lighttoys/ft/LighttoysFTProp.h"

#include "props/lightrix/LightrixFamily.h"
#include "props/lightrix/LightrixHoop.h"

#include "Prop/props/open/serial/OpenSerialProp.h"

#include "BentoEngine.h"

PropManager::PropManager() :
	BaseManager("Props"),
	familiesCC("Families")
{
	saveAndLoadRecursiveData = true;

	managerFactory = &factory;
	selectItemWhenCreated = false;

	detectProps = addTrigger("Detect Props", "Auto detect using the Yo protocol");
	autoAssignIdTrigger = addTrigger("Auto Assign IDs", "Auto assign based on order in the manager");
	sendFeedback = addBoolParameter("Send Feedback", "If checked, will send feedback from sensor to OSC", false);
	bakeAll = addTrigger("Bake All", "Bake all props");
	bakeMode = addBoolParameter("Bake Mode", "Bake Mode", false);
	powerOffAll = addTrigger("Poweroff All", "");
	resetAll = addTrigger("Reset All", "");
	clearAll = addTrigger("Clear all props", "Remove all props from manager");

	fileName = addStringParameter("Show filename", "Filename of the show", "timeline");
	loadAll = addTrigger("Load all", "Load show on all devices that can play");
	playAll = addTrigger("Play all", "Play show on all devices that can play");
	stopAll = addTrigger("Stop all", "Stop show on all devices that can stop");
	loop = addBoolParameter("Loop show", "If checked, this will tell the player to loop the playing", false);

	
	String localIp = NetworkHelpers::getLocalIP();

	addChildControllableContainer(&familiesCC);
	families.add(new FlowtoysFamily());
	families.add(new GarnavFamily());
	families.add(new LighttoysFamily());
	families.add(new LightrixFamily());

	for (auto & f : families) familiesCC.addChildControllableContainer(f);

	factory.defs.add(Factory<Prop>::Definition::createDef("Flowtoys", FlowClubProp::getTypeStringStatic(), FlowClubProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Flowtoys", "Flowtoys ESP8266 Proto", FlowClubProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Garnav", "SmartBall", SmartballProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Lighttoys", "Lighttoys FT", LighttoysFTProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Lightrix", "Lightrix Hoop", LightrixHoopProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Open", "Open Serial", OpenSerialProp::create));
	factory.defs.add(Factory<Prop>::Definition::createDef("Music", "Clarinet", BentoProp::create)->addParam("type","Clarinet")->addParam("family","Music"));

	receiver.addListener(this);

	setupReceiver();
	sender.connect("0.0.0.0", 9000);
}


PropManager::~PropManager()
{
}

void PropManager::setupReceiver()
{
	receiver.disconnect();
	bool result = receiver.connect(localPort);

	if (result)
	{
		NLOG(niceName, "Now receiving on port : " << localPort);
		clearWarning();
	}
	else
	{
		NLOGERROR(niceName, "Error binding port " << localPort);
		setWarningMessage("Error binding port " +  String(localPort));
	}
	
	Array<IPAddress> ad;
	IPAddress::findAllAddresses(ad);

	Array<String> ips;
	for (auto &a : ad) ips.add(a.toString());
	ips.sort();
	String s = "Local IPs:";
	for (auto &ip : ips) s += String("\n > ") + ip;

	NLOG(niceName, s);
}

Prop * PropManager::getPropWithHardwareId(const String &hardwareId)
{
	for (auto & p : items) if (p->deviceID == hardwareId) return p;
	return nullptr;
}

Prop * PropManager::getPropWithId(int id, Prop * excludeProp)
{
	for (auto & p : items)
	{
		if (p == excludeProp) continue;
		if (p->globalID->intValue() == id) return p;
	}
	return nullptr;
}

PropFamily * PropManager::getFamilyWithName(StringRef familyName)
{
	for (auto &p : families)
	{
		if (p->niceName == familyName) return p;
	}

	return nullptr;
}


void PropManager::onContainerTriggerTriggered(Trigger * t)
{
	if (t == autoAssignIdTrigger)
	{
		int id = 0;
		for (auto &p : items)
		{
			p->globalID->setValue(id);
			id++;
		}
	}
	else if (t == detectProps)
	{

		StringArray ips = NetworkHelpers::getLocalIPs();

		LOG("Auto detecting props");
		for (auto& ip : ips)
		{
			StringArray a;
			a.addTokens(ip, ".", "\"");
			if (a.size() < 4) continue;
			String broadcastIP = a[0] + "." + a[1] + "." + a[2] + ".255";


			OSCMessage m("/yo");
			m.addArgument(ip);
			sender.sendToIPAddress(broadcastIP, 9000, m);
			LOG(" > sending /yo on " << broadcastIP << " with local ip " << ip << "...");
		}
		

		LighttoysFTProp::autoDetectRemotes();
		((FlowtoysFamily *)getFamilyWithName("Flowtoys"))->checkSerialDevices();
	}
	else if (t == bakeAll)
	{
		for (auto & p : items) p->bakeAndUploadTrigger->trigger();
	}
	else if (t == powerOffAll)
	{
		for (auto& p : items) p->powerOffTrigger->trigger();
	}
	else if (t == resetAll)
	{
		for (auto& p : items) p->restartTrigger->trigger();
	}
	else if (t == clearAll)
	{
		Array<Prop*> itemsToRemove;
		itemsToRemove.addArray(items);
		removeItems(itemsToRemove);
	}
	else if (t == loadAll || t == playAll || t == stopAll)
	{
		for (auto& p : items)
		{
			if (BentoProp* bp = dynamic_cast<BentoProp*>(p))
			{
				if (t == loadAll) bp->loadBake(fileName->stringValue());
				else if (t == playAll) bp->playBake(0, loop->boolValue());
				else if (t == stopAll) bp->stopBakePlaying();
			}
		}
	}
}

void PropManager::onContainerParameterChanged(Parameter * p)
{
	BaseManager::onContainerParameterChanged(p);
	if (p == bakeMode)
	{
		for (auto &pr : items) pr->bakeMode->setValue(bakeMode->boolValue());
	}
}

void PropManager::onControllableFeedbackUpdate(ControllableContainer *, Controllable * c)
{
	Prop * p = ControllableUtil::findParentAs<Prop>(c);
	if (p != nullptr)
	{
		bool shouldSend = sendFeedback->boolValue() && c->parentContainer == &p->sensorsCC && c->type != Controllable::TRIGGER;

		if (shouldSend)
		{
			OSCMessage msg("/prop/" + String(p->globalID->intValue()) + "/" + c->shortName);
			msg.addArgument(OSCHelpers::varToArgument(((Parameter *)c)->getValue()));

			BentoEngine * be = (BentoEngine *)Engine::mainEngine;
			be->globalSender.sendToIPAddress(be->remoteHost->stringValue(), be->remotePort->intValue(), msg);
		}
	}
}

void PropManager::addItemInternal(Prop * p, var)
{
	p->addPropListener(this);

	if (Engine::mainEngine->isLoadingFile) return;
	if (items.size() > 1) p->globalID->setValue(getFirstAvailableID());
}

void PropManager::removeItemInternal(Prop * p)
{
	p->removePropListener(this);
}

void PropManager::clear()
{
	BaseManager::clear();
	for (auto &f : families) f->props.clear();
}

int PropManager::getFirstAvailableID()
{
	int numItems = items.size();
	for (int i = 0; i < numItems; i++)
	{
		if (getPropWithId(i) == nullptr) return i;
	}
	return numItems;
}

void PropManager::propIDChanged(Prop * p, int previousID)
{
	Prop * otherPropWithSameID = getPropWithId(p->globalID->intValue(), p);
	if (otherPropWithSameID != nullptr) otherPropWithSameID->globalID->setValue(previousID);
}

void PropManager::oscMessageReceived(const OSCMessage & m)
{
	String address = m.getAddressPattern().toString();

	if (address == "/wassup")
	{
		String pHost = OSCHelpers::getStringArg(m[0]);
		String pid = OSCHelpers::getStringArg(m[1]);
		String pType = m.size() >= 3 ? OSCHelpers::getStringArg(m[2]) : "Flowtoys Creator Club";

		DBG("Got wassup : " << pHost << " : " << pid << ", type is " << pType);
		Prop * p = getPropWithHardwareId(pid);
		if (p == nullptr)
		{
			p = static_cast<Prop *>(managerFactory->create(pType));
			if (p != nullptr)
			{
				p->deviceID = pid;
				
				BentoProp* bp = dynamic_cast<BentoProp*>(p);
				if(bp != nullptr) bp->remoteHost->setValue(pHost);
				
				LOG("Found " << p->type->getValueKey() << " with ID : " << p->deviceID);

				addItem(p);
				autoAssignIdTrigger->trigger();
			}
			else
			{
				DBG("Type does not exist " << pType);

			}
		}
		else
		{
			LOG(p->deviceID << " already there, updating prop's remoteHost");
			BentoProp* bp = dynamic_cast<BentoProp*>(p);
			if (bp != nullptr) bp->remoteHost->setValue(pHost);
		}
	}
	else  if(m.size() > 0 && m[0].isString())
	{
		if (Prop* p = getPropWithHardwareId(OSCHelpers::getStringArg(m[0])))
		{
			p->handleOSCMessage(m);
		}
	}
}