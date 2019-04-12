#include "Torus.h"
#include "../quartic.h"
#include <vector>
#include "../scene/ISect.h"

Torus::Torus(Scene* scene, Material* mat, const double a, const double b)
	: MaterialSceneObject(scene, mat),
	  a(a),
	  b(b)
{
}

bool Torus::intersectLocal(const Ray& r, ISect& iSect) const
{
	auto p = r.getPosition();
	auto d = r.getDirection();

	const auto aa = a * a;

	const auto g = 4 * aa * (d[0] * d[0] + d[1] * d[1]);
	const auto h = 8 * aa * (p[0] * d[0] + p[1] * d[1]);
	const auto i = 4 * aa * (p[0] * p[0] + p[1] * p[1]);
	const auto j = d.length_squared();
	const auto k = 2 * p.dot(d);
	const auto l = p.length_squared() + (aa - b * b);

	const auto term1 = j * j;
	const auto term2 = 2 * j * k;
	const auto term3 = 2 * j * l + k * k - g;
	const auto term4 = 2 * k * l - h;
	const auto term5 = l * l - i;

	auto* solutions = solve_quartic(term2 / term1, term3 / term1, term4 / term1, term5 / term1);
	std::vector<double> ts;
	for (auto ti = 0; ti < 4; ti++)
	{
		if (solutions[ti].imag() != 0.0 || solutions[ti].real() < 0 || solutions[ti].real() < RAY_EPSILON )
		{
			continue;
		}
		ts.push_back(solutions[ti].real());
	}
	delete[] solutions;

	if (ts.empty()) { return false; }

	std::sort(ts.begin(), ts.end());

	const auto t = iSect.t = ts[0];

	const auto pp = r.at(t);

	const auto alpha = 1.0 - a / sqrt(pp[0] * pp[0] + pp[1] * pp[1]);
	iSect.N = vec3f{alpha * pp[0], alpha * pp[1], pp[2]}.normalize();
	iSect.obj = this;

	return true;
}

bool Torus::hasBoundingBoxCapability() const
{
	return true;
}

BoundingBox Torus::computeLocalBoundingBox() const
{
	BoundingBox localBounds;
	localBounds.min = vec3f(-a - b, -a - b, -b);
	localBounds.max = vec3f(a + b, a + b, b);
	return localBounds;
}
