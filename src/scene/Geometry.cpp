#include "Geometry.h"
#include "ISect.h"

bool Geometry::intersect(const Ray&r, ISect&i, std::stack<Geometry*>& intersections) const
{
	// Transform the ray into the object's local coordinate space
	vec3f pos = transform->globalToLocalCoords(r.getPosition());
	vec3f dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
	double length = dir.length();
	dir /= length;

	Ray localRay(pos, dir);

	if (intersectLocal(localRay, i)) {
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

void Geometry::computeBoundingBox()
{
	// take the object's local bounding box, transform all 8 points on it,
	// and use those to find a new bounding box.

	const auto localBounds = computeLocalBoundingBox();

	auto min = localBounds.min;
	auto max = localBounds.max;

	auto v = transform->localToGlobalCoords(vec4f(min[0], min[1], min[2], 1));
	auto newMax = v;
	auto newMin = v;
	v = transform->localToGlobalCoords(vec4f(max[0], min[1], min[2], 1));
	newMax = maximum(newMax, v);
	newMin = minimum(newMin, v);
	v = transform->localToGlobalCoords(vec4f(min[0], max[1], min[2], 1));
	newMax = maximum(newMax, v);
	newMin = minimum(newMin, v);
	v = transform->localToGlobalCoords(vec4f(max[0], max[1], min[2], 1));
	newMax = maximum(newMax, v);
	newMin = minimum(newMin, v);
	v = transform->localToGlobalCoords(vec4f(min[0], min[1], max[2], 1));
	newMax = maximum(newMax, v);
	newMin = minimum(newMin, v);
	v = transform->localToGlobalCoords(vec4f(max[0], min[1], max[2], 1));
	newMax = maximum(newMax, v);
	newMin = minimum(newMin, v);
	v = transform->localToGlobalCoords(vec4f(min[0], max[1], max[2], 1));
	newMax = maximum(newMax, v);
	newMin = minimum(newMin, v);
	v = transform->localToGlobalCoords(vec4f(max[0], max[1], max[2], 1));
	newMax = maximum(newMax, v);
	newMin = minimum(newMin, v);

	bounds.max = vec3f(newMax);
	bounds.min = vec3f(newMin);
}

Geometry::Geometry(Scene* scene): SceneElement(scene)
{
}
