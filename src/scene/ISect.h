#pragma once
#include "SceneObject.h"

// The description of an intersection point.

class ISect
{
public:
	ISect();

	~ISect();

	void setObject(SceneObject* o);
	void setT(const double tt);
	void setN(const vec3f& n);

	void setMaterial(Material* m);

	ISect& operator =(const ISect& other);

	const SceneObject* obj;
	double t;
	vec3f N;
	Material* material; // if this intersection has its own material
	// (as opposed to one in its associated object)
	// as in the case where the material was interpolated

	const Material& getMaterial() const;
	// Other info here.
};