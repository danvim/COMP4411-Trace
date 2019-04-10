#pragma once
#include "vecmath/vecmath.h"

class Marble
{
public:
	vec3f color;
	double frequency;
	double lightness;
	double contrast;
	vec3f scale;
	int depth;

	Marble(const vec3f& color, double frequency, double lightness, double contrast, const vec3f& scale, int depth);

	vec3f getColor(const vec3f& position) const;

private:
	double getTurbulence(const vec3f& position) const;
};
