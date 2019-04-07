#include <cmath>

#include "Cylinder.h"

bool Cylinder::intersectLocal(const Ray& r, ISect& i) const
{
	i.obj = this;

	if (intersectCaps(r, i))
	{
		ISect ii;
		if (intersectBody(r, ii))
		{
			if (ii.t < i.t)
			{
				i = ii;
				i.obj = this;
			}
		}
		return true;
	}
	return intersectBody(r, i);
}

bool Cylinder::intersectBody(const Ray& r, ISect& i) const
{
	const auto x0 = r.getPosition()[0];
	const auto y0 = r.getPosition()[1];
	const auto x1 = r.getDirection()[0];
	const auto y1 = r.getDirection()[1];

	const auto a = x1 * x1 + y1 * y1;
	const auto b = 2.0 * (x0 * x1 + y0 * y1);
	const auto c = x0 * x0 + y0 * y0 - 1.0;

	if (0.0 == a)
	{
		// This implies that x1 = 0.0 and y1 = 0.0, which further
		// implies that the ray is aligned with the body of the cylinder,
		// so no intersection.
		return false;
	}

	auto discriminant = b * b - 4.0 * a * c;

	if (discriminant < 0.0)
	{
		return false;
	}

	discriminant = sqrt(discriminant);

	const auto t2 = (-b + discriminant) / (2.0 * a);

	if (t2 <= RAY_EPSILON)
	{
		return false;
	}

	const auto t1 = (-b - discriminant) / (2.0 * a);

	if (t1 > RAY_EPSILON)
	{
		// Two intersections.
		auto p = r.at(t1);
		const auto z = p[2];
		if (z >= 0.0 && z <= 1.0)
		{
			// It's okay.
			i.t = t1;
			i.N = Eigen::Vector3d(p[0], p[1], 0.0).normalized();
			return true;
		}
	}

	auto p = r.at(t2);
	const auto z = p[2];
	if (z >= 0.0 && z <= 1.0)
	{
		i.t = t2;

		Eigen::Vector3d normal(p[0], p[1], 0.0);
		// In case we are _inside_ the _uncapped_ cone, we need to flip the normal.
		// Essentially, the cone in this case is a double-sided surface
		// and has _2_ normals
		if (!capped && normal.dot(r.getDirection()) > 0)
			normal = -normal;

		i.N = normal.normalized();
		return true;
	}

	return false;
}

bool Cylinder::intersectCaps(const Ray& r, ISect& i) const
{
	if (!capped)
	{
		return false;
	}

	const auto pz = r.getPosition()[2];
	const auto dz = r.getDirection()[2];

	if (0.0 == dz)
	{
		return false;
	}

	double t1;
	double t2;

	if (dz > 0.0)
	{
		t1 = -pz / dz;
		t2 = (1.0 - pz) / dz;
	}
	else
	{
		t1 = (1.0 - pz) / dz;
		t2 = -pz / dz;
	}

	if (t2 < RAY_EPSILON)
	{
		return false;
	}

	if (t1 >= RAY_EPSILON)
	{
		auto p(r.at(t1));
		if (p[0] * p[0] + p[1] * p[1] <= 1.0)
		{
			i.t = t1;
			if (dz > 0.0)
			{
				// Intersection with cap at z = 0.
				i.N = Eigen::Vector3d(0.0, 0.0, -1.0);
			}
			else
			{
				i.N = Eigen::Vector3d(0.0, 0.0, 1.0);
			}
			return true;
		}
	}

	auto p(r.at(t2));
	if (p[0] * p[0] + p[1] * p[1] <= 1.0)
	{
		i.t = t2;
		if (dz > 0.0)
		{
			// Intersection with cap at z = 1.
			i.N = Eigen::Vector3d(0.0, 0.0, 1.0);
		}
		else
		{
			i.N = Eigen::Vector3d(0.0, 0.0, -1.0);
		}
		return true;
	}

	return false;
}
