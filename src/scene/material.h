//
// material.h
//
// The Material class: a description of the physical properties of a surface
// that are used to determine how that surface interacts with light.

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "../vecmath/vecmath.h"
#include <Eigen/Dense>

class Scene;
class Ray;
class ISect;

class Material
{
public:
	virtual ~Material() = default;

	Material()
        : ke( Eigen::Vector3d( 0.0, 0.0, 0.0 ) )
        , ka( Eigen::Vector3d( 0.0, 0.0, 0.0 ) )
        , ks( Eigen::Vector3d( 0.0, 0.0, 0.0 ) )
        , kd( Eigen::Vector3d( 0.0, 0.0, 0.0 ) )
        , kr( Eigen::Vector3d( 0.0, 0.0, 0.0 ) )
        , kt( Eigen::Vector3d( 0.0, 0.0, 0.0 ) )
        , shininess( 0.0 ) 
		, index(1.0) {}

    Material( const Eigen::Vector3d& e, const Eigen::Vector3d& a, const Eigen::Vector3d& s, 
              const Eigen::Vector3d& d, const Eigen::Vector3d& r, const Eigen::Vector3d& t, double sh, double in)
        : ke( e ), ka( a ), ks( s ), kd( d ), kr( r ), kt( t ), shininess( sh ), index( in ) {}

	virtual Eigen::Vector3d shade(Scene* scene, const Ray& r, const ISect& i) const;

    Eigen::Vector3d ke;                    // emissive
    Eigen::Vector3d ka;                    // ambient
    Eigen::Vector3d ks;                    // specular
    Eigen::Vector3d kd;                    // diffuse
    Eigen::Vector3d kr;                    // reflective
    Eigen::Vector3d kt;                    // transmissive
    
    double shininess;
    double index;               // index of refraction

    
                                // material with zero coeffs for everything
                                // as opposed to the "default" material which is
                                // a pleasant blue.
    static const Material ZERO;

    Material &
    operator+=( const Material &m )
    {
        ke += m.ke;
        ka += m.ka;
        ks += m.ks;
        kd += m.kd;
        kr += m.kr;
        kt += m.kt;
        index += m.index;
        shininess += m.shininess;
        return *this;
    }

    friend Material operator*( double d, Material m );
};

inline Material
operator*(const double d, Material m )
{
    m.ke *= d;
    m.ka *= d;
    m.ks *= d;
    m.kd *= d;
    m.kr *= d;
    m.kt *= d;
    m.index *= d;
    m.shininess *= d;
    return m;
}
// extern Material THE_DEFAULT_MATERIAL;

#endif // MATERIAL_H_
