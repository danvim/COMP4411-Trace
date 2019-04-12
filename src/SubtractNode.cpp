#include "SubtractNode.h"

bool SubtractNode::intersect(const Ray& r, ISect& i, std::stack<Geometry*>& intersections) const
{
	return a->intersect(r, i, intersections) && !b->intersect(r, i, intersections);
}

bool SubtractNode::contains(bool intersections) const
{
	return a->contains(intersections) && !b->contains(intersections);
}
