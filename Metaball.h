#pragma once
#include "src/scene/scene.h"
#include <vector>
#include "src/SceneObjects/Sphere.h"

typedef struct
{
	vec3f position;
	double strength;
} Ball;

typedef struct
{
	double t;
	bool isEntry;
	Ball ball;
} Intersect;

class Metaball : public MaterialSceneObject
{
public:
	Metaball(Scene *scene, Material *mat);
	~Metaball();

	bool isThresholdReach(const std::vector<Ball>& active, const Ray& ray, double t) const;
	bool intersectLocal(const Ray& r, ISect& i) const override;
	bool intersectBoundingSphere(const Ball& ball, const Ray& r, std::pair<double, double>& T) const ;
	std::vector<Ball> metaballs;
	double threshold = 0.25;
};

