//
// material.h
//
// The Material class: a description of the physical properties of a surface
// that are used to determine how that surface interacts with light.

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "../vecmath/vecmath.h"
#include "Texture.h"

class Scene;
class Ray;
class ISect;

class Material
{
public:
	virtual ~Material() = default;

	Material();

	Material(const vec3f& e, const vec3f& a, const vec3f& s,
	         const vec3f& d, const vec3f& r, const vec3f& t, double sh, double in);

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
	Texture* normalTexturePtr = nullptr;
	Texture* displacementTexturePtr = nullptr;

	virtual vec3f shade(Scene* scene, const Ray& r, const ISect& i) const;

	Material&
	operator+=(const Material& m);

	friend Material operator*(double d, Material m);
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
