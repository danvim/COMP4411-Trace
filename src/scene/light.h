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
	enum struct Type
	{
		kCircle = 0,
		kSquare,
		kTriangle,
		kStar
	};
	WarnLight(Scene* scene, const vec3f& pos, const vec3f dir, const vec3f& color)
		:PointLight(scene, pos, color), dir(dir)
	{
		u = dir.cross({ 0,1,0 }).normalize();
		v = dir.cross(u).normalize();
		u = dir.cross(v).normalize();
		mat4f translate({ 1,0,0,-pos[0] }, { 0,1,0,-pos[1] }, { 0,0,1,-pos[2] }, { 0,0,0,0 });
		mat4f rotate({ u[0],u[1],u[2],0 }, { v[0],v[1],v[2],0 }, { dir[0],dir[1],dir[2],0 }, { 0,0,0,1 });
		mat4f project({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { 0,0,1,0 });
		matrix = project * rotate*translate;
	}

	double distanceAttenuation(const vec3f& p) const override;
	void setType(Type type) { this->type = type; }
	vec3f dir,u,v;
	Type type = Type::kCircle;
	mat4f matrix;
	double size = 0.1;
};

#endif // LIGHT_H_
