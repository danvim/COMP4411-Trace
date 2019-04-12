#include "IntersectionNode.h"
#include "scene/ISect.h"

IntersectionNode::
IntersectionNode(Scene* const scene, SceneObject* const a, SceneObject* const b): BooleanNode(scene, a, b)
{
}

bool IntersectionNode::intersect(const Ray& r, ISect& i, std::stack<Geometry*>& intersections) const
{
	i.obj = a;
	auto aISect = i;
	auto bISect = i;
	const auto localRay = getLocalRay(r);
	const auto isAIntersects = a->intersect(localRay, aISect, intersections);
	const auto isBIntersects = b->intersect(localRay, bISect, intersections);
	if (!(isAIntersects && isBIntersects))
	{
		return false;
	}

	Geometry* nearObj;
	Geometry* farObj;
	ISect* nearISect;

	// Take closer intersection
	if (aISect.t < bISect.t)
	{
		nearObj = a;
		farObj = b;
		nearISect = &aISect;
	}
	else
	{
		nearObj = b;
		farObj = a;
		nearISect = &bISect;
	}

	if (!intersections.empty() && intersections.top() == nearObj)
	{
		intersections.pop();
	}
	else
	{
		intersections.push(nearObj);
		const auto inRay = Ray(localRay.at(nearISect->t + RAY_EPSILON), localRay.getDirection());
		const auto intersects = farObj->intersect(inRay, i, intersections);
		i.obj = a;
		return intersects;
	}

	return false;
}

bool IntersectionNode::contains(bool intersections) const
{
	return a->contains(intersections) && b->contains(intersections);
}
