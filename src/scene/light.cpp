#include "light.h"

double DirectionalLight::distanceAttenuation( const Eigen::Vector3d& p ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


Eigen::Vector3d DirectionalLight::shadowAttenuation( const Eigen::Vector3d& p ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    return Eigen::Vector3d(1,1,1);
}

Eigen::Vector3d DirectionalLight::getColor( const Eigen::Vector3d& p ) const
{
	// Color doesn't depend on P 
	return color;
}

Eigen::Vector3d DirectionalLight::getDirection( const Eigen::Vector3d& p ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const Eigen::Vector3d& p ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	return 1.0;
}

Eigen::Vector3d PointLight::getColor( const Eigen::Vector3d& p ) const
{
	// Color doesn't depend on P 
	return color;
}

Eigen::Vector3d PointLight::getDirection( const Eigen::Vector3d& p ) const
{
	return (position - p).normalized();
}


Eigen::Vector3d PointLight::shadowAttenuation(const Eigen::Vector3d& p) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    return Eigen::Vector3d(1,1,1);
}
