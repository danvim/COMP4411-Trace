#pragma once
#include "../scene/scene.h"

// From: http://cosinekitty.com/raytrace/chapter13_torus.html

class Torus final : public MaterialSceneObject
{
public:
	double a;
	double b;

	Torus(Scene* scene, Material* mat, double a, double b);
	;

	bool intersectLocal(const Ray& r, ISect& iSect) const override;
	bool hasBoundingBoxCapability() const override;
	BoundingBox ComputeLocalBoundingBox() const override;
};
