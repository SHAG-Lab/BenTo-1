/*
==============================================================================

NodeConnectionSlot.h
Created: 14 Apr 2018 5:45:15pm
Author:  Ben

==============================================================================
*/

#pragma once

#include "JuceHeader.h"
class Prop;
class Node;
class NodeConnection;

enum ConnectionType { Color, ColorBlock, Number };

class NodeConnectionSlot
{
public:
	NodeConnectionSlot(Node * node, bool isInput, const String &name, ConnectionType type) : node(node), isInput(isInput), name(name), type(type) {}
	~NodeConnectionSlot() { masterReference.clear(); }

	Node * node;
	bool isInput;
	String name;
	ConnectionType type;

	Array<NodeConnection *> connections;
	bool isConnected();
	
	void addConnection(NodeConnection * c);
	void removeConnection(NodeConnection *c);

	WeakReference<NodeConnectionSlot>::Master masterReference;
};

class ColorSlot :
	public NodeConnectionSlot
{
public:
	ColorSlot(Node * node, bool isInput, const String &name) : NodeConnectionSlot(node, isInput, name, ConnectionType::ColorBlock) {}

	Array<Colour> getColors(Prop * p, double time, var params);
};

class ParameterSlot :
	public NodeConnectionSlot
{
public:
	ParameterSlot(Node * node, bool isInput, Parameter * p) :
		NodeConnectionSlot(node, isInput, p->niceName, p->type == Parameter::COLOR ? ConnectionType::Color : ConnectionType::Number),
		parameter(p)
	{
	}
	 
	var getValue();
	Parameter * parameter;
};