#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"

class Camera
{
public:
    Camera();
    void rayThrough( double x, double y, Ray &r ) const;
    void setEye( const Eigen::Vector3d &eye );
    void setLook( double, double, double, double );
    void setLook( const Eigen::Vector3d &viewDir, const Eigen::Vector3d &upDir );
    void setFov( double );
    void setAspectRatio( double );

    double getAspectRatio() const { return aspectRatio; }
private:
    Eigen::Matrix3d m;                     // rotation matrix
    double normalizedHeight;    // dimensions of image place at unit dist from eye
    double aspectRatio;
    
    void update();              // using the above three values calculate look,u,v
    
    Eigen::Vector3d eye;
    Eigen::Vector3d look;                  // direction to look
    Eigen::Vector3d u,v;                   // u and v in the 
};

#endif
