#include "light.h"
#include <vector>
#include "ISect.h"

extern std::vector<vec3f> sampleDistributed(vec3f c, double r, int count);

double DirectionalLight::distanceAttenuation( const vec3f& p ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


Light::Light(Scene* scene, const vec3f& col): SceneElement(scene), color(col)
{
}

DirectionalLight::DirectionalLight(Scene* scene, const vec3f& orien, const vec3f& color): Light(scene, color),
                                                                                          orientation(orien)
{
}

vec3f DirectionalLight::shadowAttenuation(const vec3f& p, std::stack<Geometry*>& intersections) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.

	Scene* pScene = getScene();
	vec3f d = getDirection(p);
	ISect i;
	Ray r(p, d);
	vec3f ret = color;
	if(pScene->intersect(r, i, intersections))
	{
		ret = prod(color, i.getMaterial().kt);
	}
	if (getScene()->softShadow)
	{
		std::vector<vec3f> vecs = sampleDistributed(d, 0.05, 49);
		for (vec3f v : vecs)
		{
			Ray r(p, v);
			ISect i;
			if (scene->intersect(r, i, intersections))
			{
				ret += prod(color, i.getMaterial().kt);
			}
			else
			{
				ret += color;
			}
		}
		ret /= 50.0;
	}
    return ret;
}

vec3f DirectionalLight::getColor( const vec3f& p ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& p ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& p ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	double d = vec3f(p - position).length();
	Scene* pScene = getScene();
	double a = pScene->distAtteA, b = pScene->distAtteB, c = pScene->distAtteC;
	double ret = 1.0 / (a + b * d + c * d * d);
	return std::min(1.0, ret);
}

vec3f PointLight::getColor( const vec3f& p ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& p ) const
{
	return (position - p).normalize();
}


PointLight::PointLight(Scene* scene, const vec3f& pos, const vec3f& color): Light(scene, color), position(pos)
{
}

vec3f PointLight::shadowAttenuation(const vec3f& p, std::stack<Geometry*>& intersections) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	Scene* pScene = getScene();
	vec3f d = getDirection(p);
	ISect i;
	Ray r(p, d);
	vec3f ret = color;
	if (pScene->intersect(r, i, intersections))
	{
		ret = prod(color,i.getMaterial().kt);
	}
	if (getScene()->softShadow)
	{
		std::vector<vec3f> vecs = sampleDistributed(d, 0.05, 49);
		for (vec3f v : vecs)
		{
			Ray r(p, v);
			ISect i;
			if (scene->intersect(r, i, intersections))
			{
				ret += prod(color, i.getMaterial().kt);
			}
			else
			{
				ret += color;
			}
		}
		ret /= 50.0;
	}
	return ret;
}
