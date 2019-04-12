#include "BooleanNode.h"

BooleanNode::~BooleanNode()
{
	delete a;
	a = nullptr;
	delete b;
	b = nullptr;
}

const Material& BooleanNode::getMaterial() const
{
	return a->getMaterial();
}

void BooleanNode::setMaterial(Material* m)
{
	return a->setMaterial(m);
}
