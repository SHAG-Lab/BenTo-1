/*
  ==============================================================================

	ModelNode.cpp
	Created: 13 Apr 2018 11:30:06pm
	Author:  Ben

  ==============================================================================
*/

#include "ModelNode.h"
#include "ui/ModelNodeUI.h"
#include "LightBlock/model/LightBlockModelLibrary.h"
#include "Prop/Prop.h"

ModelNode::ModelNode(var params) :
	ColorNode(getTypeString(), params),
	currentBlock(nullptr)
{
	activeProvider = addTargetParameter("Active Block", "The current active block for this prop");
	activeProvider->targetType = TargetParameter::CONTAINER;
	activeProvider->customGetTargetContainerFunc = &LightBlockModelLibrary::showProvidersAndGet;
}

ModelNode::~ModelNode()
{
}


void ModelNode::setBlockFromProvider(LightBlockColorProvider * provider)
{
	if (currentBlock == nullptr && provider == nullptr) return;
	if (provider != nullptr && currentBlock != nullptr && currentBlock->provider == provider) return;

	if (currentBlock != nullptr)
	{
		removeChildControllableContainer(currentBlock);
		currentBlock = nullptr;
	}

	if (provider != nullptr) currentBlock = new LightBlock(provider);


	if (currentBlock != nullptr)
	{
		addChildControllableContainer(currentBlock);
	}
}

Array<Colour> ModelNode::getColors(Prop * p, double time, var params)
{
	if (currentBlock == nullptr) return ColorNode::getColors(p, time, params);
	return currentBlock->getColors(p, time, params);
}

void ModelNode::onContainerParameterChanged(Parameter * p)
{
	if (p == activeProvider)
	{
		setBlockFromProvider(dynamic_cast<LightBlockColorProvider *>(activeProvider->targetContainer.get()));
	}
}

NodeViewUI * ModelNode::createUI()
{
	return new ModelNodeUI(this);
}
