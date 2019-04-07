#ifndef CYLINDER_H_
#define CYLINDER_H_

#include "../scene/scene.h"

class Cylinder
	: public MaterialSceneObject
{
public:
	Cylinder( Scene *scene, Material *mat , const bool cap = true)
		: MaterialSceneObject( scene, mat ), capped( cap )
	{
	}

	bool intersectLocal( const Ray& r, ISect& i ) const override;
	bool hasBoundingBoxCapability() const override { return true; }

	BoundingBox computeLocalBoundingBox() override
    {
        BoundingBox localBounds;
		localBounds.min = Eigen::Vector3d(-1.0f, -1.0f, 0.0f);
		localBounds.max = Eigen::Vector3d(1.0f, 1.0f, 1.0f);
        return localBounds;
    }

    bool intersectBody( const Ray& r, ISect& i ) const;
	bool intersectCaps( const Ray& r, ISect& i ) const;

protected:
	bool capped;
};

#endif // CYLINDER_H_
