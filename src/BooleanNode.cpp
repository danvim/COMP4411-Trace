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

Ray BooleanNode::getLocalRay(const Ray& r) const
{
	const auto pos = transform->globalToLocalCoords(r.getPosition());
	const auto dir = (transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos).normalize();

	return Ray(pos, dir);
}
