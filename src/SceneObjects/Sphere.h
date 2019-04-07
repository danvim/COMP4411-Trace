#ifndef SPHERE_H_
#define SPHERE_H_

#include "../scene/scene.h"

class Sphere
	: public MaterialSceneObject
{
public:
	Sphere( Scene *scene, Material *mat )
		: MaterialSceneObject( scene, mat )
	{
	}

	bool intersectLocal( const Ray& r, ISect& i ) const override;
	bool hasBoundingBoxCapability() const override { return true; }

	BoundingBox ComputeLocalBoundingBox() const override
    {
        BoundingBox localBounds;
		localBounds.min = vec3f(-1.0f, -1.0f, -1.0f);
		localBounds.max = vec3f(1.0f, 1.0f, 1.0f);
        return localBounds;
    }
};
#endif // SPHERE_H_
