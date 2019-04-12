#ifndef SQUARE_H_
#define SQUARE_H_
#include "../scene/MaterialSceneObject.h"


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

	std::pair<double, double> getUV(const Ray& r, const ISect& i) const override;

	BoundingBox computeLocalBoundingBox() const override
    {
        BoundingBox localBounds;
        localBounds.min = vec3f(-0.5f, -0.5f, -RAY_EPSILON);
		localBounds.max = vec3f(0.5f, 0.5f, RAY_EPSILON);
        return localBounds;
    }

	static vec3f getP(const Ray& r, const ISect& i);
};

#endif // SQUARE_H_
