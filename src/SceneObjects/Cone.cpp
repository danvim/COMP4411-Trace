#include <cmath>

#include "Cone.h"

bool Cone::intersectLocal(const Ray& r, ISect& i) const
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


bool Cone::intersectBody(const Ray& r, ISect& i) const
{
	auto d = r.getDirection();
	auto p = r.getPosition();

	const auto a = d[0] * d[0] + d[1] * d[1] - C * d[2] * d[2];
	const auto b = 2.0 * (d[0] * p[0] + d[1] * p[1] - C * d[2] * p[2]) - B * d[2];
	const auto c = p[0] * p[0] + p[1] * p[1] - A - B * p[2] - C * p[2] * p[2];

	auto disc = b * b - 4.0 * a * c;

	if (disc <= 0.0)
	{
		return false;
	}

	disc = sqrt(disc);

	const auto t1 = (-b - disc) / (2.0 * a);
	const auto t2 = (-b + disc) / (2.0 * a);

	if (t2 < RAY_EPSILON)
	{
		return false;
	}

	if (t1 > RAY_EPSILON)
	{
		// Two intersections.
		auto pp = r.at(t1);
		const auto z = pp[2];
		if (z >= 0.0 && z <= height)
		{
			// It's okay.
			i.t = t1;
			i.N = Eigen::Vector3d(pp[0], pp[1],
			                      -(c * pp[2] + (tRadius - bRadius) * tRadius / height)).normalized();


			return true;
		}
	}

	auto ppp = r.at(t2);
	const auto z = ppp[2];
	if (z >= 0.0 && z <= height)
	{
		i.t = t2;
		i.N = Eigen::Vector3d(ppp[0], ppp[1],
		                      -(c * ppp[2] + (tRadius - bRadius) * tRadius / height)).normalized();
		// In case we are _inside_ the _uncapped_ cone, we need to flip the normal.
		// Essentially, the cone in this case is a double-sided surface
		// and has _2_ normals

		if (!capped && i.N.dot(r.getDirection()) > 0)
			i.N = -i.N;

		return true;
	}

	return false;
}

bool Cone::intersectCaps(const Ray& r, ISect& i) const
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
	double r1;
	double r2;

	if (dz > 0.0)
	{
		t1 = -pz / dz;
		t2 = (height - pz) / dz;
		r1 = bRadius;
		r2 = tRadius;
	}
	else
	{
		t1 = (height - pz) / dz;
		t2 = -pz / dz;
		r1 = tRadius;
		r2 = bRadius;
	}

	if (t2 < RAY_EPSILON)
	{
		return false;
	}

	if (t1 >= RAY_EPSILON)
	{
		auto p(r.at(t1));
		if (p[0] * p[0] + p[1] * p[1] <= r1 * r1)
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
	if (p[0] * p[0] + p[1] * p[1] <= r2 * r2)
	{
		i.t = t2;
		if (dz > 0.0)
		{
			// Intersection with interior of cap at z = 1.
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
