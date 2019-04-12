//
// ray.h
//
// The low-level classes used by ray tracing: the ray and isect classes.
//

#ifndef RAY_H_
#define RAY_H_

#include "../vecmath/vecmath.h"

class SceneObject;

// A ray has a position where the ray starts, and a direction (which should
// always be normalized!)

class Ray
{
public:
	Ray(const vec3f& pp, const vec3f& dd)
		: p(pp), d(dd)
	{
	}

	Ray(const Ray& other) = default;
	~Ray() = default;

	Ray& operator =(const Ray& other)
	= default;

	vec3f at(const double t) const
	{
		return p + t * d;
	}

	vec3f getPosition() const { return p; }
	vec3f getDirection() const { return d; }

protected:
	vec3f p;
	vec3f d;
};

const double RAY_EPSILON = 0.00001;
const double NORMAL_EPSILON = 0.00001;

#endif // RAY_H_
