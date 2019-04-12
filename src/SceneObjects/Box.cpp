#include "Box.h"

extern double diff(double, double);

bool Box::intersectLocal( const Ray& r, ISect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.

	BoundingBox b = computeLocalBoundingBox();
	double tMin, tMax;
	if(!b.intersect(r, tMin, tMax))
	{
		return false;
	}
	if(tMin < RAY_EPSILON || tMax < RAY_EPSILON || diff(tMax,tMin) < RAY_EPSILON)
	{
		// ray origin inside box OR box behind ray OR ray tangent to box
		return false;
	}
	i.t = tMin;
	vec3f p = r.at(i.t);
	i.N = vec3f(0, 0, 0);
	if (diff(p[0], 0.5) < RAY_EPSILON)
		i.N[0] = 1;
	else if (diff(p[0], -0.5) < RAY_EPSILON)
		i.N[0] = -1;
	else if (diff(p[1], 0.5) < RAY_EPSILON)
		i.N[1] = 1;
	else if (diff(p[1], -0.5) < RAY_EPSILON)
		i.N[1] = -1;
	else if (diff(p[2], 0.5) < RAY_EPSILON)
		i.N[2] = 1;
	else if (diff(p[2], -0.5) < RAY_EPSILON)
		i.N[2] = -1;
	
	if (r.getDirection() * i.N > 0)
		i.N = -i.N;
	
	i.obj = this;
	
	return true;
}
