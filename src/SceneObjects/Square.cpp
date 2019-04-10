#include "Square.h"

bool Square::intersectLocal(const Ray& r, ISect& i) const
{
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	if (d[2] == 0.0)
	{
		return false;
	}

	double t = -p[2] / d[2];

	if (t <= RAY_EPSILON)
	{
		return false;
	}

	vec3f P = r.at(t);

	if (P[0] < -0.5 || P[0] > 0.5)
	{
		return false;
	}

	if (P[1] < -0.5 || P[1] > 0.5)
	{
		return false;
	}

	i.obj = this;
	i.t = t;
	if (d[2] > 0.0)
	{
		i.N = vec3f(0.0, 0.0, -1.0);
	}
	else
	{
		i.N = vec3f(0.0, 0.0, 1.0);
	}

	return true;
}

std::pair<double, double> Square::getUV(const Ray& r, const ISect& i) const
{
	auto bigP = transform->globalToLocalCoords(getP(r, i));

	if (bigP[0] < -0.5 || bigP[0] > 0.5)
	{
		return {};
	}

	if (bigP[1] < -0.5 || bigP[1] > 0.5)
	{
		return {};
	}

	return {std::clamp(bigP[0] + 0.5, 0.0, 1.0), std::clamp(bigP[1] + 0.5, 0.0, 1.0)};
}

vec3f Square::getP(const Ray& r, const ISect& i)
{
	auto p = r.getPosition();
	auto d = r.getDirection();
	const auto t = -p[2] / d[2];
	if (t <= RAY_EPSILON)
	{
		return {};
	}
	return r.at(t);
}
