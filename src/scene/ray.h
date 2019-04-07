//
// ray.h
//
// The low-level classes used by ray tracing: the ray and isect classes.
//

#ifndef RAY_H_
#define RAY_H_

#include <utility>
#include "Eigen/Dense"
#include "material.h"

class SceneObject;

// A ray has a position where the ray starts, and a direction (which should
// always be normalized!)

class Ray
{
public:
	Ray(Eigen::Vector3d pp, Eigen::Vector3d dd)
		: p(std::move(pp)), d(std::move(dd))
	{
	}

	Ray(const Ray& other) = default;
	~Ray() = default;

	Ray& operator =(const Ray& other)
	= default;

	Eigen::Vector3d at(const double t) const
	{
		return p + t * d;
	}

	Eigen::Vector3d getPosition() const { return p; }
	Eigen::Vector3d getDirection() const { return d; }

protected:
	Eigen::Vector3d p;
	Eigen::Vector3d d;
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
	void setN(const Eigen::Vector3d& n) { N = n; }

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

	const SceneObject* obj;
	double t;
	Eigen::Vector3d N;
	Material* material; // if this intersection has its own material
	// (as opposed to one in its associated object)
	// as in the case where the material was interpolated

	const Material& getMaterial() const;
	// Other info here.
};

const double RAY_EPSILON = 0.00001;
const double NORMAL_EPSILON = 0.00001;

#endif // RAY_H_
