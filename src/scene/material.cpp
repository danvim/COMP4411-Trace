#include <cmath>
#include <algorithm>
#include "ray.h"
#include "material.h"
#include "light.h"

// the color of that point.
vec3f Material::shade(Scene* scene, const Ray& r, const ISect& i) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
	// You will need to call both distanceAttenuation() and shadowAttenuation()
	// somewhere in your code in order to compute shadows and light falloff.

	static const vec3f ONES = { 1.0, 1.0, 1.0 };

	vec3f V = r.getDirection();
	vec3f N = i.N;
	vec3f P = r.at(i.t);
	
	vec3f color = ke + prod(ka, scene->ambientLight);

	auto diffuseColor = kd;
	auto specularColor = ks;
	vec3f opacity = ONES - kt;

	const auto* obj = dynamic_cast<const MaterialSceneObject*>(i.obj);
	if (obj != nullptr)
	{
		// obj is type of MaterialSceneObject
		// do uv mapping
		auto [u, v] = obj->getUV(r, i);
		if (diffuseTexturePtr != nullptr)
		{
			diffuseColor = diffuseTexturePtr->getColorByUV(u, v);
		}
		if (specularTexturePtr != nullptr)
		{
			specularColor = specularTexturePtr->getColorByUV(u, v);
		}
		if (transmissionTexturePtr != nullptr)
		{
			opacity = ONES - transmissionTexturePtr->getColorByUV(u, v);
		}
		// if (normalTexturePtr != nullptr)
		// {
		// 	const auto diffFromUp = N - UP;
		// 	N = (normalTexturePtr->getColorByUV(u, v) + diffFromUp).normalize();
		// }
	}

	for (auto l = scene->beginLights(); l != scene->endLights(); ++l)
	{
		Light* pLight = *l;
		vec3f L = pLight->getDirection(P);
		vec3f intensity = pLight->getColor(P);
		const double distAtte = pLight->distanceAttenuation(P);
		vec3f shadowAtte = pLight->shadowAttenuation(P);

		const double diffuse = std::max(0.0, N.dot(L));

		vec3f R = L - 2 * L.dot(N) * N;
		R = R.normalize();
		const double specAngle = std::max(R.dot(V), 0.0);
		const double specular = pow(specAngle, shininess * 128);

		vec3f ret = prod(distAtte * (specular * specularColor + prod(diffuse * diffuseColor, opacity)), intensity);
		ret = prod(ret, shadowAtte);
		color += ret;
	}


	for (auto j = 0; j < 3; j++) color[j] = std::min(color[j], 1.0);
	return color;
}

Material& Material::operator+=(const Material& m)
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

int Material::cnt = 0;
