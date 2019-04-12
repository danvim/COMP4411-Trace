#ifndef CONE_H_
#define CONE_H_
#include "../scene/MaterialSceneObject.h"


class Cone
	: public MaterialSceneObject
{
public:
	Cone(Scene* scene, Material* mat,
	     const double h = 1.0, const double br = 1.0, const double tr = 0.0,
	     const bool cap = false)
		: MaterialSceneObject(scene, mat)
	{
		height = h;
		bRadius = br < 0.0f ? -br : br;
		tRadius = tr < 0.0f ? -tr : tr;
		capped = cap;

		computeAbc();
	}

	bool intersectLocal(const Ray& r, ISect& i) const override;
	bool hasBoundingBoxCapability() const override { return true; }

	BoundingBox computeLocalBoundingBox() const override
	{
		BoundingBox localBounds;
		const auto biggestRadius = bRadius > tRadius ? bRadius : tRadius;

		localBounds.min = vec3f(-biggestRadius, -biggestRadius, height < 0.0f ? height : 0.0f);
		localBounds.max = vec3f(biggestRadius, biggestRadius, height < 0.0f ? 0.0f : height);
		return localBounds;
	}

	bool intersectBody(const Ray& r, ISect& i) const;
	bool intersectCaps(const Ray& r, ISect& i) const;

	bool isFullyEnclosed() const override;


protected:
	void computeAbc()
	{
		A = bRadius * bRadius;
		B = 2.0 * bRadius * (tRadius - bRadius) / height;
		C = (tRadius - bRadius) / height;
		C = C * C;
	}

	bool capped;
	double height;
	double bRadius;
	double tRadius;

	double A;
	double B;
	double C;
};

#endif // CONE_H_
