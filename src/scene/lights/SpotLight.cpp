#include "SpotLight.h"
#include "../../utils.h"
#include <cassert>

SpotLight::SpotLight(Scene* scene, const vec3f& color, const vec3f& position, const vec3f& orientation,
                     const double size,
                     const double blend):
	PointLight(scene, position, color),
	orientation(orientation),
	size(size),
	blend(blend)
{
	assert(size >= 0.0);
	assert(blend >= 0.0 && blend <= 1.0);

	cosOuterCone = cos(size / 2);
	cosInnerCone = cos(innerCone());
}

vec3f SpotLight::shadowAttenuation(const vec3f& p) const
{
	auto* pScene = getScene();
	const auto d = getDirection(p);
	const auto cosDirection = d.dot(-orientation);
	const auto spotEffect = hermiteInterpolation(cosOuterCone, cosInnerCone, cosDirection);

	ISect intersection;
	const Ray ray(p, d);
	auto ret = color * spotEffect;
	if (pScene->intersect(ray, intersection))
	{
		ret = prod(intersection.getMaterial().kt, ret);
	}
	return ret;
}

double SpotLight::innerCone() const
{
	return size * (1 - blend) / 2;
}
