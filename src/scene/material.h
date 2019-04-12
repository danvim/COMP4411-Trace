//
// material.h
//
// The Material class: a description of the physical properties of a surface
// that are used to determine how that surface interacts with light.

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <stack>
#include "../vecmath/vecmath.h"
#include "Texture.h"
#include "../Marble.h"

class Scene;
class Ray;
class ISect;
class Geometry;

class Material
{
public:
	virtual ~Material() = default;

	Material()
        : ke( vec3f( 0.0, 0.0, 0.0 ) )
        , ka( vec3f( 0.0, 0.0, 0.0 ) )
        , ks( vec3f( 0.0, 0.0, 0.0 ) )
        , kd( vec3f( 0.0, 0.0, 0.0 ) )
        , kr( vec3f( 0.0, 0.0, 0.0 ) )
        , kt( vec3f( 0.0, 0.0, 0.0 ) )
        , shininess( 0.0 ) 
		, index(1.0) {
		id = cnt++;
	}

    Material( const vec3f& e, const vec3f& a, const vec3f& s, 
              const vec3f& d, const vec3f& r, const vec3f& t, double sh, double in)
		: ke(e), ka(a), ks(s), kd(d), kr(r), kt(t), shininess(sh), index(in) {
		id = cnt++;
	}

	static Material getAir()
	{
		return Material({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, { 1,1,1 }, 0, 1);
	}

	vec3f ke; // emissive
	vec3f ka; // ambient
	vec3f ks; // specular
	vec3f kd; // diffuse
	vec3f kr; // reflective
	vec3f kt; // transmissive
	double shininess;
	double index; // index of refraction

	Texture* diffuseTexturePtr = nullptr;
	Texture* specularTexturePtr = nullptr;
	Texture* emissionTexturePtr = nullptr;
	Texture* reflectivityTexturePtr = nullptr;
	Texture* transmissionTexturePtr = nullptr;
	Texture* normalTexturePtr = nullptr;
	Texture* ambientTexturePtr = nullptr;
	Texture* displacementTexturePtr = nullptr;

	Marble* diffuseMarblePtr = nullptr;

	virtual vec3f shade(Scene* scene, const Ray& r, const ISect& i, std::stack<Geometry*>& intersections) const;

	Material&
	operator+=(const Material& m);


	friend Material operator*(double d, Material m);

	int id;
	static int cnt;

};

inline Material operator*(const double d, Material m)
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
