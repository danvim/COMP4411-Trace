#include "camera.h"

#define PI 3.14159265359
#define SHOW(x) (cerr << #x << " = " << (x) << "\n")

Camera::Camera()
{
    aspectRatio = 1;
    normalizedHeight = 1;
    
    eye = Eigen::Vector3d(0,0,0);
    u = Eigen::Vector3d( 1,0,0 );
    v = Eigen::Vector3d( 0,1,0 );
    look = Eigen::Vector3d( 0,0,-1 );
}

void
Camera::rayThrough( double x, double y, Ray &r ) const
// Ray through normalized window point x,y.  In normalized coordinates
// the camera's x and y vary both vary from 0 to 1.
{
    x -= 0.5;
    y -= 0.5;
    Eigen::Vector3d dir = look + x * u + y * v;
    r = Ray( eye, dir.normalized() );
}

void
Camera::setEye( const Eigen::Vector3d &eye )
{
    this->eye = eye;
}

void
Camera::setLook( double r, double i, double j, double k )
// Set the direction for the camera to look using a quaternion.  The
// default camera looks down the neg z axis with the pos y axis as up.
// We derive the new look direction by rotating the camera by the
// quaternion r i j k.
{
                                // set look matrix
	Eigen::Matrix3d mm;

	mm << 1.0 - 2.0 * (i * i + j * j), 2.0 * (r * i - j * k), 2.0 * (j * r + i * k),
		2.0 * (r * i + j * k), 1.0 - 2.0 * (j * j + r * r), 2.0 * (i * j - r * k),
		2.0 * (j * r - i * k), 2.0 * (i * j + r * k), 1.0 - 2.0 * (i * i + r * r);

	m = mm;

    update();
}

void
Camera::setLook( const Eigen::Vector3d &viewDir, const Eigen::Vector3d &upDir )
{
	const auto z = -viewDir;          // this is where the z axis should end up
    const auto& y = upDir;      // where the y axis should end up
	const auto x = y.cross(z);               // lah,

	Eigen::Matrix3d m;

	m << x, y, z;

	m.transposeInPlace();

    update();
}

void
Camera::setFov( double fov )
// fov - field of view (height) in degrees    
{
    fov /= (180.0 / PI);      // convert to radians
    normalizedHeight = 2 * tan( fov / 2 );
    update();
}

void
Camera::setAspectRatio(const double ar )
// ar - ratio of width to height
{
    aspectRatio = ar;
    update();
}

void
Camera::update()
{
    u = m * Eigen::Vector3d( 1,0,0 ) * (normalizedHeight * aspectRatio);
    v = m * Eigen::Vector3d( 0,1,0 ) * normalizedHeight;
    look = m * Eigen::Vector3d( 0,0,-1 );
}




