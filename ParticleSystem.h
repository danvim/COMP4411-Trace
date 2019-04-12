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
	//q: particle pos, p: ray pos, d: ray dir
	bool intersectParticle(vec3f& q, vec3f& p, vec3f& d)const {
		vec3f pq = (q - p).normalize();
		return (abs(abs(pq * d) - 1) < RAY_EPSILON);
	}
};

