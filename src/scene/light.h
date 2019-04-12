#ifndef LIGHT_H_
#define LIGHT_H_

#include "scene.h"

class Light : public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& p) const = 0;
	virtual double distanceAttenuation(const vec3f& p) const = 0;
	virtual vec3f getColor(const vec3f& p) const = 0;
	virtual vec3f getDirection(const vec3f& p) const = 0;

protected:
	Light(Scene* scene, const vec3f& col)
		: SceneElement(scene), color(col)
	{
	}

	vec3f color;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(Scene* scene, const vec3f& orien, const vec3f& color)
		: Light(scene, color), orientation(orien)
	{
	}

	vec3f shadowAttenuation(const vec3f& p) const override;
	double distanceAttenuation(const vec3f& p) const override;
	vec3f getColor(const vec3f& p) const override;
	vec3f getDirection(const vec3f& p) const override;

protected:
	vec3f orientation;
};

class PointLight : public Light
{
public:
	PointLight(Scene* scene, const vec3f& pos, const vec3f& color)
		: Light(scene, color), position(pos)
	{
	}

	vec3f shadowAttenuation(const vec3f& p) const override;
	double distanceAttenuation(const vec3f& p) const override;
	vec3f getColor(const vec3f& p) const override;
	vec3f getDirection(const vec3f& p) const override;

protected:
	vec3f position;
};

class WarnLight : public PointLight
{
public:
	WarnLight(Scene* scene, const vec3f& pos, const vec3f dir, const vec3f& color)
		:PointLight(scene, pos, color), dir(dir)
	{
		u = dir.cross({ 0,1,0 }).normalize();
		v = dir.cross(u).normalize();
		u = dir.cross(v).normalize();
	}

	double distanceAttenuation(const vec3f& p) const override;
	vec3f dir,u,v;
};

#endif // LIGHT_H_
