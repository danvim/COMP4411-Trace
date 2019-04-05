//
// ray.h
//
// The low-level classes used by ray tracing: the ray and isect classes.
//

#ifndef RAY_H_
#define RAY_H_

#include "../vecmath/vecmath.h"
#include "material.h"

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

// The description of an intersection point.

class ISect
{
public:
	ISect()
		: obj(nullptr), t(0.0), material(nullptr)
	{
	}

	~ISect()
	{
		delete material;
	}

	void setObject(SceneObject* o) { obj = o; }
	void setT(const double tt) { t = tt; }
	void setN(const vec3f& n) { N = n; }

	void setMaterial(Material* m)
	{
		delete material;
		material = m;
	}

	ISect& operator =(const ISect& other)
	{
		if (this != &other)
		{
			obj = other.obj;
			t = other.t;
			N = other.N;
			//            material = other.material ? new Material( *(other.material) ) : 0;
			if (other.material)
			{
				if (material)
					*material = *other.material;
				else
					material = new Material(*other.material);
			}
			else
			{
				material = nullptr;
			}
		}
		return *this;
	}

public:
	const SceneObject* obj;
	double t;
	vec3f N;
	Material* material; // if this intersection has its own material
	// (as opposed to one in its associated object)
	// as in the case where the material was interpolated

	const Material& getMaterial() const;
	// Other info here.
};

const double RAY_EPSILON = 0.00001;
const double NORMAL_EPSILON = 0.00001;

#endif // RAY_H_
