#ifndef LIGHT_H_
#define LIGHT_H_

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual Eigen::Vector3d shadowAttenuation(const Eigen::Vector3d& p) const = 0;
	virtual double distanceAttenuation(const Eigen::Vector3d& p) const = 0;
	virtual Eigen::Vector3d getColor(const Eigen::Vector3d& p) const = 0;
	virtual Eigen::Vector3d getDirection(const Eigen::Vector3d& p) const = 0;

protected:
	Light(Scene* scene, const Eigen::Vector3d& col)
		: SceneElement(scene), color(col)
	{
	}

	Eigen::Vector3d color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight(Scene* scene, const Eigen::Vector3d& orien, const Eigen::Vector3d& color)
		: Light(scene, color), orientation(orien)
	{
	}

	Eigen::Vector3d shadowAttenuation(const Eigen::Vector3d& p) const override;
	double distanceAttenuation(const Eigen::Vector3d& p) const override;
	Eigen::Vector3d getColor(const Eigen::Vector3d& p) const override;
	Eigen::Vector3d getDirection(const Eigen::Vector3d& p) const override;

protected:
	Eigen::Vector3d orientation;
};

class PointLight
	: public Light
{
public:
	PointLight(Scene* scene, const Eigen::Vector3d& pos, const Eigen::Vector3d& color)
		: Light(scene, color), position(pos)
	{
	}

	Eigen::Vector3d shadowAttenuation(const Eigen::Vector3d& p) const override;
	double distanceAttenuation(const Eigen::Vector3d& p) const override;
	Eigen::Vector3d getColor(const Eigen::Vector3d& p) const override;
	Eigen::Vector3d getDirection(const Eigen::Vector3d& p) const override;

protected:
	Eigen::Vector3d position;
};

#endif // LIGHT_H_
