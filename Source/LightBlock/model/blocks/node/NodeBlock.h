/*
  ==============================================================================

    NodeBlock.h
    Created: 10 Apr 2018 6:59:08pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "../../LightBlockModel.h"
#include "Node/NodeManager.h"

class NodeBlock :
	public LightBlockModel,
	public NodeManager::Listener
{
public:
	NodeBlock(var params = var());
	~NodeBlock();

	NodeManager manager;

	HashMap<Parameter *, Parameter *> paramToNodeMap;

	virtual Array<WeakReference<Controllable>> getModelParameters() override;
	Array<Colour> getColors(Prop * p, double time, var params) override;

	void itemAdded(Node *) override;
	void itemRemoved(Node *) override;
	void childAddressChanged(ControllableContainer * cc) override;
	void childStructureChanged(ControllableContainer * cc) override;

	void updateParametersFromNode();

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	String getTypeString() const override { return "Node"; }
	static NodeBlock * create(var params) { return new NodeBlock(params); }

	LightBlockModelUI * createUI() override;
};