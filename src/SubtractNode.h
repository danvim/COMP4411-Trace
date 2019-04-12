#pragma once
#include "BooleanNode.h"

class SubtractNode: public BooleanNode
{
public:
	SubtractNode(Scene* const scene, SceneObject* const a, SceneObject* const b);

	bool intersect(const Ray& r, ISect& i, std::stack<Geometry*>& intersections) const override;
	bool contains(bool intersections) const override;
};
