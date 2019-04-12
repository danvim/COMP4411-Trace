#include "Metaball.h"



Metaball::Metaball(Scene *scene, Material *mat):MaterialSceneObject(scene, mat)
{
	metaballs.push_back({ {0,0,0},1 });
	metaballs.push_back({ {1,0.5,0},1 });
	metaballs.push_back({ {0,0.9,0},1 });
	// metaballs.push_back({ {2,2,2},3 });
	// metaballs.push_back({ {0,1,0},2 });
}


Metaball::~Metaball()
{
}

bool Metaball::isThresholdReach(const std::vector<Ball>& active, const Ray& ray, double t) const
{
	vec3f p = ray.at(t);
	double d = 0;
	for(auto&& ball: active)
	{
		double r = (ball.position - p).length();
		// d += 1 / r / r;
		d += (1 - r * r * r * (r * (r * 6 - 15) + 10)) * ball.strength;
	}
	return d > threshold;
}

bool Metaball::intersectLocal(const Ray& r, ISect& i) const
{
	std::vector<Intersect> intersections;
	for(auto&& ball: metaballs)
	{
		std::pair<double, double> T;
		bool isIntersect = intersectBoundingSphere(ball, r, T);
		if (isIntersect)
		{
			intersections.push_back({ T.first,true, ball });
			intersections.push_back({ T.second,false, ball });
		}
	}
	std::sort(intersections.begin(), intersections.end(),[](const Intersect& a, const Intersect& b)
	{
		return a.t < b.t;
	});
	std::vector<Ball> active;
	int cnt = 0;
	for(auto&& intersect: intersections)
	{
		cnt++;
		if(intersect.isEntry)
		{
			active.push_back(intersect.ball);
		}else
		{
			for(auto it = active.begin(); it!=active.end(); ++it)
			{
				if(&*it == &intersect.ball)
				{
					active.erase(it);
					break;
				}
			}
			if (cnt == intersections.size())break;
		}
		int nSteps = 1000;
		double dt = (intersections[cnt].t - intersect.t)/double(nSteps);
		double t = intersect.t;
		for(int k=0; k<nSteps; k++)
		{
			if(isThresholdReach(active,r,t))
			{
				i.obj = this;
				i.N = r.at(t);
				i.t = t;
				return true;
			}
			t += dt;
		}
	}
	return false;
}

bool Metaball::intersectBoundingSphere(const Ball& ball, const Ray& ray, std::pair<double,double>& T) const
{
	vec3f d = ray.getDirection();
	vec3f ballP = ball.position;
	vec3f rayP = ray.getPosition();
	double r = ball.strength;
	double a = d.length_squared();
	double b = 2 * d.dot(rayP - ballP);
	double c = rayP.dot(rayP) - 2 * rayP.dot(ballP) + ballP.dot(ballP) - r*r;

	double discriminant = b * b - 4 * a*c;
	if (discriminant < 0) {
		return false;
	}

	discriminant = sqrt(discriminant);
	double t2 = -b + discriminant;
	t2 /= 2 * a;

	if (t2 <= RAY_EPSILON) {
		return false;
	}


	double t1 = -b - discriminant;
	t1 /= 2 * a;

	if (t1 > RAY_EPSILON) {
		T.first = t1;
		T.second = t2;
	}
	else {
		T.first = t2;
		T.second = t1;
	}

	return true;
}
