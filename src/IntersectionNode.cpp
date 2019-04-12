#include "IntersectionNode.h"

IntersectionNode::
IntersectionNode(Scene* const scene, SceneObject* const a, SceneObject* const b): BooleanNode(scene, a, b)
{
}

bool IntersectionNode::intersect(const Ray& r, ISect& i, std::stack<Geometry*>& intersections) const
{
	auto aISect = i;
	auto bISect = i;
	const auto isAIntersects = a->intersect(r, aISect, intersections);
	const auto isBIntersects = b->intersect(r, bISect, intersections);
	const auto isIntersects = isAIntersects && isBIntersects;
	Geometry* x = nullptr;
	Geometry* y = nullptr;

	std::tie(x, y) = aISect.t < bISect.t ? std::pair<Geometry*, Geometry*>(a, b) : std::pair<Geometry*, Geometry*>(b, a);

	if (intersections.top() == x)
	{
		intersections.pop();
	}
	else
	{
		intersections.push(x);
		const auto localRay = Ray(r.at(aISect.t + RAY_EPSILON), r.getDirection());
		return b->intersect(localRay, i, intersections);
	}

	return isIntersects;
}

bool IntersectionNode::contains(bool intersections) const
{
	return a->contains(intersections) && b->contains(intersections);
}
