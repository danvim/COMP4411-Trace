#pragma once
#include "src/scene/scene.h"
#include <vector>

extern double drand(double low, double high);
extern int irand(int low, int high);

typedef struct
{
	vec3f position;
	vec3f velocity;
	vec3f color;
	double decay;
} Particle;

class ParticleSystem : public MaterialSceneObject
{
public:

	ParticleSystem(Scene *scene, Material *mat);
	~ParticleSystem();
	std::vector<Particle> particles;
	void set(vec3f origin, vec3f dir, vec3f acceleration, vec3f initColor, double speedMin, double speedMax, double decayMin, double decayMax, int framesCnt, int particlesCnt);
	void init();

	Particle generateRandomParticle();

	bool intersectLocal(const Ray& r, ISect& i) const override;
	vec3f origin, dir, acceleration, initColor, u,v;
	double speedMin, speedMax, decayMin, decayMax;
	int framesCnt, particlesCnt;
	bool equal(vec3f& postionParticle, vec3f& positionRay, vec3f& direction)const {
		vec3f dir = (postionParticle - positionRay).normalize();
		if (abs(abs(dir * direction) - 1) < 0.00001)
			return true;
		return false;
	}
};

