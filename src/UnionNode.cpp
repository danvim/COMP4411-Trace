#include "UnionNode.h"

bool UnionNode::intersect(const Ray& r, ISect& i, std::stack<Geometry*>& intersections) const
{
	return a->intersect(r, i, intersections) || b->intersect(r, i, intersections);
}

bool UnionNode::contains(bool intersections) const
{
	return a->contains(intersections) || b->contains(intersections);
}
