#pragma once
#include "BooleanNode.h"

class IntersectionNode: public BooleanNode
{
public:
	IntersectionNode(Scene* scene, SceneObject* a, SceneObject* b);

	bool intersect(const Ray& r, ISect& i, std::stack<Geometry*>& intersections) const override;

	bool contains(bool intersections) const override;
};
