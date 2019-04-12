#include "ParticleSystem.h"
#include "src/scene/ISect.h"

ParticleSystem::ParticleSystem(Scene *scene, Material *mat)
	: MaterialSceneObject(scene, mat)
{
	origin = { 0,0,0 };
	dir = { 0,1,0 };
	u = { 1,0,0 };
	v = { 0,0,1 };
	acceleration = { 0,-0.005,0 };
	initColor = { 0,0,1 };
	speedMin = 0.01;
	speedMax = 0.1;
	decayMin = 1;
	decayMax = 1;
	particlesCnt = 1000;
	framesCnt = 40;
}


ParticleSystem::~ParticleSystem()
{
}

Particle ParticleSystem::generateRandomParticle()
{
	vec3f position = origin + drand(-0.05, 0.05)*u + drand(-0.05, 0.05)*v;
	vec3f velocity = (dir + drand(-0.25, 0.25)*u + drand(-0.25, 0.25)*v) * drand(speedMin, speedMax);
	double decay = drand(decayMin, decayMax);
	vec3f color;
	switch(irand(0,3))
	{
	case 0:
	case 1:
		color = initColor;
		break;
	case 2:
		color = { 1,1,1 };
		break;
	}
	color*=drand(0.6, 1);
	Particle particle = { position, velocity, color,decay };
	return particle;
}

void ParticleSystem::init()
{
	particles.clear();
	for(int i=0;i<particlesCnt; i++)
	{
		Particle particle = generateRandomParticle();
		int t = irand(0,framesCnt);
		particle.position += particle.velocity * t + acceleration * t * t / 2;
		particle.color *= pow(particle.decay, t);
		particles.push_back(particle);
	}
	particles.push_back({ {0,0,0},{0,1,0},{1,1,1},1 });
	particles.push_back({ {0,1,0},{0,1,0},{1,1,1},1 });
	particles.push_back({ {0,0,1},{0,1,0},{1,1,1},1 });
	particles.push_back({ {1,0,0},{0,1,0},{1,1,1},1 });
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
		if(intersectParticle(q,p,d)){
		// if((r.at(t)-q).length() - 5< RAY_EPSILON){
		// if(abs(abs(dd.dot(d))-1)<RAY_EPSILON){
			isIntersect = true;
			double t = (q - p).length();
			minT = std::min(minT, t);
			for (int j = 0; j < 3; j++)color[j] = std::max(color[j], particle.color[j]);
		}
	}
	if(isIntersect)
	{
		Material* m = new Material;
		m->ke = color;
		i.t = minT;
		i.N = -d;
		i.material = m;
	}
	return isIntersect;
}