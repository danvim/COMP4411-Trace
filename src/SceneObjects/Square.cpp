#include "Square.h"

bool Square::intersectLocal(const Ray& r, ISect& i) const
{
	auto p = r.getPosition();
	auto d = r.getDirection();

	if (d[2] == 0.0)
	{
		return false;
	}

	const auto t = -p[2] / d[2];

	if (t <= RAY_EPSILON)
	{
		return false;
	}

	auto pp = r.at(t);

	if (pp[0] < -0.5 || pp[0] > 0.5)
	{
		return false;
	}

	if (pp[1] < -0.5 || pp[1] > 0.5)
	{
		return false;
	}

	i.obj = this;
	i.t = t;
	if (d[2] > 0.0)
	{
		i.n = Eigen::Vector3d(0.0, 0.0, -1.0);
	}
	else
	{
		i.n = Eigen::Vector3d(0.0, 0.0, 1.0);
	}

	return true;
}
