#include "SubtractNode.h"
#include "scene/ISect.h"

SubtractNode::SubtractNode(Scene* const scene, SceneObject* const a, SceneObject* const b): BooleanNode(scene, a, b)
{
}

bool SubtractNode::intersect(const Ray& r, ISect& i, std::stack<Geometry*>& intersections) const
{
	i.obj = a;
	auto aISect = i;
	auto bISect = i;
	const auto localRay = getLocalRay(r);
	const auto isAIntersects = a->intersect(localRay, aISect, intersections);
	const auto isBIntersects = b->intersect(localRay, bISect, intersections);
	if (!isAIntersects)
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

	if (!isBIntersects || nearObj == a)
	{
		i = aISect;
		return true;
	}
	else if (nearObj == b)
	{
		const auto inRay = Ray(localRay.at(bISect.t + RAY_EPSILON), localRay.getDirection());
		const auto intersectsA = a->intersect(inRay, aISect, intersections);
		const auto intersectsB = b->intersect(inRay, bISect, intersections);
		if (intersectsA && intersectsB)
		{
			auto aISect2 = aISect;
			const auto inRay2 = Ray(inRay.at(aISect.t + RAY_EPSILON), localRay.getDirection());
			a->intersect(inRay2, aISect2, intersections);
			if (bISect.t < aISect2.t)
			{
				i = bISect;
				i.N *= -1;
				i.obj = a;
				return true;
			}

			return false;
		}
		return false;
	}
	

	return false;
}

bool SubtractNode::contains(bool intersections) const
{
	return a->contains(intersections) && !b->contains(intersections);
}
