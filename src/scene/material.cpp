#include <cmath>
#include <algorithm>
#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const Ray& r, const ISect& i ) const
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

	vec3f V = r.getDirection();
	vec3f N = i.N;
	vec3f P = r.at(i.t);
	vec3f transparent = vec3f(1, 1, 1) - kt;
	vec3f color = ke+ prod(ka, scene->ambientLight);

	for (std::list<Light*>::const_iterator l = scene->beginLights(); l != scene->endLights(); ++l) {
		Light *pLight = *l;
		vec3f L = pLight->getDirection(P);
		vec3f intensity = pLight->getColor(P);
		double distAtte = pLight->distanceAttenuation(P);
		vec3f shadowAtte = pLight->shadowAttenuation(P);

		double diffuse = std::max(0.0, N.dot(L));

		double specular = 0;
		vec3f R = L - 2 * L.dot(N) * N;
		R = R.normalize();
		double specAngle = std::max(R.dot(V), 0.0);
		specular = pow(specAngle, shininess * 128);

		vec3f ret = prod(distAtte *(specular * ks + prod(diffuse * kd,transparent)), intensity);
		ret = prod(ret, shadowAtte);
		color += ret;
	}


	for (int i = 0; i < 3; i++) color[i] = std::min(color[i], 1.0);
	return color;
}


int Material::cnt = 0;