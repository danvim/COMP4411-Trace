#include <cmath>

#include "scene.h"
#include "../ui/TraceUI.h"
#include "light.h"
extern TraceUi* traceUi;

// Does this bounding box intersect the target?
bool BoundingBox::intersects(const BoundingBox& target) const
{
	return target.min[0] - RAY_EPSILON <= max[0] && target.max[0] + RAY_EPSILON >= min[0] &&
		target.min[1] - RAY_EPSILON <= max[1] && target.max[1] + RAY_EPSILON >= min[1] &&
		target.min[2] - RAY_EPSILON <= max[2] && target.max[2] + RAY_EPSILON >= min[2];
}

// does the box contain this point?
bool BoundingBox::intersects(const Eigen::Vector3d& point) const
{
	return point[0] + RAY_EPSILON >= min[0] && point[1] + RAY_EPSILON >= min[1] && point[2] + RAY_EPSILON >= min[2] &&
		point[0] - RAY_EPSILON <= max[0] && point[1] - RAY_EPSILON <= max[1] && point[2] - RAY_EPSILON <= max[2];
}

// if the ray hits the box, put the "t" value of the intersection
// closest to the origin in tMin and the "t" value of the far intersection
// in tMax and return true, else return false.
// Using Kay/Kajiya algorithm.
bool BoundingBox::intersect(const Ray& r, double& tMin, double& tMax) const
{
	auto r0 = r.getPosition();
	auto rd = r.getDirection();

	tMin = -1.0e308; // 1.0e308 is close to infinity... close enough for us!
	tMax = 1.0e308;

	for (auto currentAxis = 0; currentAxis < 3; currentAxis++)
	{
		const auto vd = rd[currentAxis];

		// if the ray is parallel to the face's plane (=0.0)
		if (vd == 0.0)
			continue;

		const auto v1 = min[currentAxis] - r0[currentAxis];
		const auto v2 = max[currentAxis] - r0[currentAxis];

		// two slab intersections
		auto t1 = v1 / vd;
		auto t2 = v2 / vd;

		if (t1 > t2)
		{
			// swap t1 & t2
			const auto tt = t1;
			t1 = t2;
			t2 = tt;
		}

		if (t1 > tMin)
			tMin = t1;
		if (t2 < tMax)
			tMax = t2;

		if (tMin > tMax) // box is missed
			return false;
		if (tMax < 0.0) // box is behind ray
			return false;
	}
	return true; // it made it past all 3 axes.
}


bool Geometry::intersect(const Ray& r, ISect& i) const
{
	// Transform the ray into the object's local coordinate space
	const auto pos = transform->globalToLocalCoords(r.getPosition());
	Eigen::Vector3d dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
	const auto length = dir.norm();
	dir /= length;

	const Ray localRay(pos, dir);

	if (intersectLocal(localRay, i))
	{
		// Transform the intersection point & normal returned back into global space.
		i.N = transform->localToGlobalCoordsNormal(i.N);
		i.t /= length;

		return true;
	}
	return false;
}

bool Geometry::intersectLocal(const Ray& r, ISect& i) const
{
	return false;
}

bool Geometry::hasBoundingBoxCapability() const
{
	// by default, primitives do not have to specify a bounding box.
	// If this method returns true for a primitive, then either the ComputeBoundingBox() or
	// the ComputeLocalBoundingBox() method must be implemented.

	// If no bounding box capability is supported for an object, that object will
	// be checked against every single ray drawn.  This should be avoided whenever possible,
	// but this possibility exists so that new primitives will not have to have bounding
	// boxes implemented for them.

	return false;
}

Scene::~Scene()
{
	GeometryIter g;

	for (g = objects.begin(); g != objects.end(); ++g)
	{
		delete *g;
	}

	for (g = boundedObjects.begin(); g != boundedObjects.end(); ++g)
	{
		delete *g;
	}

	for (g = unboundedObjects.begin(); g != boundedObjects.end(); ++g)
	{
		delete *g;
	}

	for (auto l = lights.begin(); l != lights.end(); ++l)
	{
		delete *l;
	}
}

// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect(const Ray& r, ISect& i) const
{
	typedef std::list<Geometry*>::const_iterator iter;
	iter j;

	ISect cur;
	auto haveOne = false;

	// try the non-bounded objects
	for (j = unboundedObjects.begin(); j != unboundedObjects.end(); ++j)
	{
		if ((*j)->intersect(r, cur))
		{
			if (!haveOne || cur.t < i.t)
			{
				i = cur;
				haveOne = true;
			}
		}
	}

	// try the bounded objects
	for (j = boundedObjects.begin(); j != boundedObjects.end(); ++j)
	{
		if ((*j)->intersect(r, cur))
		{
			if (!haveOne || cur.t < i.t)
			{
				i = cur;
				haveOne = true;
			}
		}
	}


	return haveOne;
}

void Scene::initScene()
{
	auto firstBoundedObject = true;

	typedef std::list<Geometry*>::const_iterator iter;
	// split the objects into two categories: bounded and non-bounded
	for (iter j = objects.begin(); j != objects.end(); ++j)
	{
		if ((*j)->hasBoundingBoxCapability())
		{
			boundedObjects.push_back(*j);

			// widen the scene's bounding box, if necessary
			if (firstBoundedObject)
			{
				sceneBounds = (*j)->getBoundingBox();
				firstBoundedObject = false;
			}
			else
			{
				auto b = (*j)->getBoundingBox();
				sceneBounds.max = sceneBounds.max.cwiseMax(b.max);
				sceneBounds.min = sceneBounds.min.cwiseMin(b.min);
			}
		}
		else
			unboundedObjects.push_back(*j);
	}
}
