#include <cmath>

#include "Sphere.h"

bool Sphere::intersectLocal( const Ray& r, ISect& i ) const
{
	Eigen::Vector3d v = -r.getPosition();
	const auto b = v.dot(r.getDirection());
	auto discriminant = b * b - v.dot(v) + 1;

	if( discriminant < 0.0 ) {
		return false;
	}

	discriminant = sqrt( discriminant );
	const auto t2 = b + discriminant;

	if( t2 <= RAY_EPSILON ) {
		return false;
	}

	i.obj = this;

	const auto t1 = b - discriminant;

	if( t1 > RAY_EPSILON ) {
		i.t = t1;
		i.n = r.at( t1 ).normalized();
	} else {
		i.t = t2;
		i.n = r.at( t2 ).normalized();
	}

	return true;
}

