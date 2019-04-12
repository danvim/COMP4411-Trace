#include "light.h"
#include <vector>
#include <corecrt_math_defines.h>

extern std::vector<vec3f> sampleDistributed(vec3f c, double r, int count);

double DirectionalLight::distanceAttenuation( const vec3f& p ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& p ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.

	Scene* pScene = getScene();
	vec3f d = getDirection(p);
	ISect i;
	Ray r(p, d);
	vec3f ret = color;
	if(pScene->intersect(r, i))
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
			if (scene->intersect(r, i))
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


vec3f PointLight::shadowAttenuation(const vec3f& p) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	Scene* pScene = getScene();
	vec3f d = getDirection(p);
	ISect i;
	Ray r(p, d);
	vec3f ret = color;
	if (pScene->intersect(r, i))
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
			if (scene->intersect(r, i))
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

double WarnLight::distanceAttenuation(const vec3f& p) const
{
	// vec3f d = (p - position).normalize();
	// double length = 1.f / (d.dot(dir));
	// vec3f dvec = d * length;
	// dvec -= dir * length*d.dot(dir);
	// length = dvec.length();
	// double x = length * dvec.dot(u);
	// double y = length * dvec.dot(v);
	double c, m;
	// std::cout << x << y;
	vec4f P = matrix * vec4f({ p[0],p[1],p[2],1 });
	double x = P[0];
	double y = P[1];
	bool show = false;
	switch(type)
	{
	case Type::kCircle:
		show = x * x + y * y < size*size; break;
	case Type::kSquare:
		show = x > -size && x< size && y>-size && y < size; break;
	case Type::kTriangle:
		c =  size;
		m = tan(M_PI / 3);
		show = y > -size/2 && y < m*x + c && y < -m * x + c; break;
	case Type::kStar:
		c = size;
		m = tan(M_PI / 3);
		show = y > -size / 2 && y < m*x + c && y < -m * x + c;
		show = show || (y < size / 2 && y > m*x - c && y > -m * x - c);
		break;
	default:
		show = false;
	}
	if(show)
	{
		return PointLight::distanceAttenuation(p);
	}else
	{
		return 0;
	}
}
