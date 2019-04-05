#ifndef SQUARE_H_
#define SQUARE_H_

#include "../scene/scene.h"

class Square
	: public MaterialSceneObject
{
public:
	Square( Scene *scene, Material *mat )
		: MaterialSceneObject( scene, mat )
	{
	}

	bool intersectLocal( const Ray& r, ISect& i ) const override;
	bool hasBoundingBoxCapability() const override { return true; }

	BoundingBox ComputeLocalBoundingBox() override
    {
        BoundingBox localBounds;
        localBounds.min = vec3f(-0.5f, -0.5f, -RAY_EPSILON);
		localBounds.max = vec3f(0.5f, 0.5f, RAY_EPSILON);
        return localBounds;
    }
};

#endif // SQUARE_H_
