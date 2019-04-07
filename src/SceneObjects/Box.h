#ifndef BOX_H_
#define BOX_H_

#include "../scene/scene.h"

class Box
	: public MaterialSceneObject
{
public:
	Box( Scene *scene, Material *mat )
		: MaterialSceneObject( scene, mat )
	{
	}

	bool intersectLocal( const Ray& r, ISect& i ) const override;
	bool hasBoundingBoxCapability() const override { return true; }

	BoundingBox computeLocalBoundingBox() override
    {
        BoundingBox localBounds;
        localBounds.max = Eigen::Vector3d(0.5, 0.5, 0.5);
		localBounds.min = Eigen::Vector3d(-0.5, -0.5, -0.5);
        return localBounds;
    }
};

#endif // BOX_H_
