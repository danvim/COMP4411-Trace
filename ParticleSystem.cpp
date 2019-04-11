#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(Scene *scene, Material *mat)
	: MaterialSceneObject(scene, mat)
{
	origin = { 0,0,0 };
	dir = { 0,1,0 };
	u = { 1,0,0 };
	v = { 0,0,1 };
	acceleration = { 0,0,0 };
	initColor = { 1,0,0 };
	speedMin = 0.005;
	speedMax = 0.05;
	decayMin = 0.95;
	decayMax = 0.99;
	particlesCnt = 10000;
}


ParticleSystem::~ParticleSystem()
{
}

Particle ParticleSystem::generateRandomParticle()
{
	vec3f position = origin + drand(-0.05, 0.05)*u + drand(-0.05, 0.05)*v;
	vec3f velocity = (dir + drand(-0.05, 0.05)*u + drand(-0.05, 0.05)*v) * drand(speedMin, speedMax);
	double decay = drand(decayMin, decayMax);
	Particle particle = { position, velocity, initColor,decay };
	return particle;
}

void ParticleSystem::init()
{
	particles.clear();
	for(int i=0;i<particlesCnt; i++)
	{
		Particle particle = generateRandomParticle();
		particle.position += particle.velocity * framesCnt + acceleration * framesCnt * framesCnt / 2;
		particle.color *= pow(particle.decay, framesCnt);
		particles.push_back(particle);
	}
}

bool ParticleSystem::intersectLocal(const Ray& r, ISect& i) const
{
	vec3f color(0, 0, 0);
	bool isIntersect = false;
	double minT = 999999;
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();
	for(auto&& particle: particles)
	{
		vec3f q = particle.position;
		double t = p.dot(q) / d.dot(q);
		if((r.at(t)-q).length()<1)
		{
			isIntersect = true;
			minT = std::min(minT, t);
			for (int j = 0; j < 3; j++)color[j] = std::max(color[j], particle.color[j]);
		}
	}
	if(isIntersect)
	{
		Material* m = new Material;
		m->kd = color;
		i.t = minT;
		i.N = -d;
		i.material = m;
	}
	return isIntersect;
}