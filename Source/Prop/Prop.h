/*
  ==============================================================================

    Prop.h
    Created: 10 Apr 2018 6:59:25pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "LightBlock/LightBlock.h"

class Prop :
	public BaseItem,
	public Inspectable::InspectableListener,
	public Thread
{
public:
	enum Shape { CLUB, BALL, POI, HOOP };

	Prop(const String &name = "Prop", var params = var());
	virtual ~Prop();

	IntParameter * id;
	IntParameter * resolution;
	EnumParameter * shape;

	FloatParameter * battery;
	BoolParameter * findPropMode;

	Array<Colour> colors;

	String propId;

	ScopedPointer<LightBlock> currentBlock;

	TargetParameter * activeProvider; 

	virtual void clearItem() override;

	void setBlockFromProvider(LightBlockColorProvider * model);

	void update();

	void onContainerParameterChanged(Parameter * p) override;
	void inspectableDestroyed(Inspectable *) override;

	void sendColorsToProp(bool forceSend = false);
	virtual void sendColorsToPropInternal() {}

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;


	//Listener
	class  PropListener
	{
	public:
		/** Destructor. */
		virtual ~PropListener() {}
		virtual void propBlockChanged(Prop * /*prop*/) {}
		virtual void colorsUpdated(Prop *) {}
	};

	ListenerList<PropListener> propListeners;
	void addPropListener(PropListener* newListener) { propListeners.add(newListener); }
	void removePropListener(PropListener* listener) { propListeners.remove(listener); }
	
	// ASYNC
	class  PropEvent
	{
	public:
		enum Type { BLOCK_CHANGED, COLORS_UPDATED };

		PropEvent(Type t, Prop * p, var v = var()) :
			type(t), prop(p), value(v) {}

		Type type;
		Prop * prop;
		var value;
	};

	QueuedNotifier<PropEvent> propNotifier;
	typedef QueuedNotifier<PropEvent>::Listener AsyncListener;

	void addAsyncPropListener(AsyncListener* newListener) { propNotifier.addListener(newListener); }
	void addAsyncCoalescedPropListener(AsyncListener* newListener) { propNotifier.addAsyncCoalescedListener(newListener); }
	void removeAsyncPropListener(AsyncListener* listener) { propNotifier.removeListener(listener); }

	// Inherited via Thread
	virtual void run() override;
};