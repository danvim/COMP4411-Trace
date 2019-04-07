#ifndef SQUARE_H_
#define SQUARE_H_

#include "../scene/scene.h"

class Square
	: public MaterialSceneObject
{
public:
	Square(Scene* scene, Material* mat)
		: MaterialSceneObject(scene, mat)
	{
	}

	bool intersectLocal(const Ray& r, ISect& i) const override;
	bool hasBoundingBoxCapability() const override { return true; }

	BoundingBox computeLocalBoundingBox() override
	{
		BoundingBox localBounds;
		localBounds.min = Eigen::Vector3d(-0.5f, -0.5f, -RAY_EPSILON);
		localBounds.max = Eigen::Vector3d(0.5f, 0.5f, RAY_EPSILON);
		return localBounds;
	}
};

#endif // SQUARE_H_
