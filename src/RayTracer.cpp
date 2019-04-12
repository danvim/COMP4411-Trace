// The main ray tracer.

#include <FL/fl_ask.H>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "SceneObjects/trimesh.h"
#include "SceneObjects/Square.h"

extern std::vector<vec3f> sampleDistributed(vec3f c, double r, int count);

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace(Scene* scene, double x, double y)
{
	Ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
	scene->getCamera()->rayThrough(x, y, r);
	materials = std::stack<Material>();
	materials.push(Material::getAir());
	auto intersections = std::stack<Geometry*>();
	vec3f color = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0, materials, intersections).clamp();

	if (getScene()->fod)
	{
		double f = getScene()->focalLength;
		vec3f fPt = r.getPosition() + f * r.getDirection();
		std::vector<vec3f> vecs = sampleDistributed(r.getDirection(), 0.05 * getScene()->aperture, 19);
		for (vec3f v : vecs)
		{
			vec3f ro = fPt - f / (v.dot(r.getDirection())) * v;
			color += traceRay(scene, Ray(ro, v), {0, 0, 0}, std::max(0, maxDepth), materials, intersections);
		}
		color /= 20.0;

		// int amount = 20;
		// double aperture = getScene()->aperture;
		// for (int i = 0; i < amount; i++)
		// {
		// 	Ray ray({ 0,0,0 }, { 0,0,0 });
		// 	float dx = frand()*aperture * 2 - aperture, dy = frand()*aperture * 2 - aperture;
		// 	scene->getCamera()->rayThrough(x + dx, y + dy, ray);
		// 	Ray
		// }
	}

	if (getScene()->motionBlur)
	{
		// std::vector<vec3f> vecs = sampleDistributed(r.getDirection(), 0.05, 49);
		vec3f c = r.getDirection();
		vec3f up = vec3f(0, 1, 0);
		if ((c.normalize() - up).length() < RAY_EPSILON)
		{
			up = vec3f(0, 0, 1);
		}
		vec3f u = (c.cross(up)).normalize();
		vec3f v = (u.cross(c)).normalize();
		u = (c.cross(v)).normalize();
		vec3f dir = c;
		for (int i = 0; i < 10; i++)
		{
			dir += 0.005 * v;
			Ray ray(r.getPosition(), dir.normalize());
			color += traceRay(scene, ray, vec3f(1.0, 1.0, 1.0), 0, materials, intersections);
		}
		color /= 10.f;
	}
	return color;
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay(Scene* scene, const Ray& r,
                          const vec3f& thresh, int depth, std::stack<Material> materials, std::stack<Geometry*>& intersections)
{
	ISect i;
	if (depth > maxDepth)return vec3f(0, 0, 0);
	if (scene->intersect(r, i, intersections))
	{
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.



		//reflection
		vec3f N = i.N;
		vec3f V = r.getDirection();
		vec3f P = r.at(i.t);
		const auto& m = i.getMaterial();
		if (materials.top().id == m.id)
		{
			N = -N;
		}

		// normal map
		static const vec3f UP = {0.0, 0.0, 1.0};
		auto* obj = dynamic_cast<const MaterialSceneObject*>(i.obj);
		if (obj != nullptr)
		{
			auto& mNormal = obj->getMaterial();
			auto [u, v] = obj->getUV(r, i);
			if (mNormal.normalTexturePtr != nullptr)
			{
				const auto diffFromUp = N - UP;
				const auto fakeNormal = mNormal.normalTexturePtr->getNormalByUV(u, v);
				N = (fakeNormal + diffFromUp).normalize();
			}
		}
		// ---

		vec3f R = V - 2 * V.dot(N) * N;
		R = R.normalize();
		vec3f phongColor = m.shade(scene, r, i, intersections);

		//Dynamic threshold
		if (scene->terminationThreshold > phongColor.length())
		{
			return phongColor;
		}
		Ray reflectRay(P, R);
		vec3f reflectColor = traceRay(scene, reflectRay, thresh, depth + 1, materials, intersections);
		reflectColor = prod(reflectColor, m.kr);
		if (getScene()->glossyReflection)
		{
			std::vector<vec3f> vecs = sampleDistributed(R, 0.05, 49);
			int depthR = std::max(depth + 1, maxDepth - 1);
			for (vec3f v : vecs)
			{
				Ray reflectRayL(P, v);
				reflectColor += prod(traceRay(scene, reflectRayL, thresh, depthR, materials, intersections), m.kr);
			}
			reflectColor /= 50.f;
		}

		vec3f refractColor(0, 0, 0);
		if (m.kt.length() > 0)
		{
			double n1 = materials.top().index, n2;
			if (materials.top().id == m.id)
			{
				//leaving current
				materials.pop();
				n2 = materials.top().index;
			}
			else
			{
				//entering m
				materials.push(m);
				n2 = m.index;
			}
			vec3f T = refraction2(V, N, n1, n2);
			// if((T - A).length() > RAY_EPSILON)
			// {
			// 	std::cout << V<<std::endl<<N<<std::endl<<n1<<" "<<n2<<std::endl<<T << std::endl << A << std::endl;
			// }
			// T = A;
			Ray refractRay(P, T);
			refractColor = traceRay(scene, refractRay, thresh, depth + 1, materials, intersections);
		}

		refractColor = prod(refractColor, m.kt);

		return phongColor + reflectColor + refractColor;
	}
	// No intersection.  This ray travels to infinity, so we color
	// it according to the background color, which in this (simple) case
	// is just black.

	return vec3f(0.0, 0.0, 0.0);
}

vec3f RayTracer::refraction2(vec3f i, vec3f n, double n1, double n2)
{
	if (abs(abs(n * i) - 1) < RAY_EPSILON)
		return i;

	double sinTheta1 = sqrt(1 - pow(n * i, 2));
	double sinTheta2 = (n1 * sinTheta1) / n2;
	double theta1 = asin(sinTheta1);
	double theta2 = asin(sinTheta2);
	double sinTheta3 = sin(abs(theta1 - theta2));

	if (n1 == n2)
	{
		return i;
	}
	else if (n1 > n2)
	{
		double critical = n2 / n1;

		if (critical - sinTheta1 > RAY_EPSILON)
		{
			double sinAlpha = sin(3.1416 - theta2);
			double fac = sinAlpha / sinTheta3;

			return -(-i * fac + (-n)).normalize();
		}
		else
		{
			//TIR
			return vec3f(0.0, 0.0, 0.0);
		}
	}
	else
	{
		double fac = sinTheta2 / sinTheta3;
		return (i * fac + (-n)).normalize();
	}
}

// math from https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
vec3f RayTracer::refraction(vec3f i, vec3f n, double n1, double n2)
{
	if (n1 == n2)return i;
	double thetaI = acos(i.dot(n));
	//total internal reflection
	if (sin(thetaI) > n2 / n1)
	{
		return vec3f(0, 0, 0);
	}
	double sinSqThetaT = pow(n1 / n2, 2) * (1 - pow(n.dot(i), 2));
	vec3f t = (n1 / n2) * i + (n1 / n2 * n.dot(i) - sqrt(1 - sinSqThetaT)) * n;
	// vec3f t = n1 / n2 * (
	// 	(sqrt(pow(n.dot(i), 2) + pow(n2 / n1, 2) - 1) - n.dot(i)) * n + i
	// 	);
	return t; // .normalize();
}

RayTracer::RayTracer()
{
	buffer = nullptr;
	bufferWidth = bufferHeight = 256;
	scene = nullptr;

	mBSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer(unsigned char*& buf, int& w, int& h) const
{
	buf = buffer;
	w = bufferWidth;
	h = bufferHeight;
}

double RayTracer::aspectRatio() const
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded() const
{
	return mBSceneLoaded;
}

void RayTracer::initializeBackgroundObj()
{
	static const vec3f ONES = {1.0, 1.0, 1.0};
	auto* backgroundMaterialPtr = new Material();
	backgroundMaterialPtr->ke = ONES;
	backgroundMaterialPtr->kd = ONES;
	backgroundMaterialPtr->ks = ONES;
	backgroundMaterialPtr->shininess = 0.6;
	backgroundObjPtr = new Square(scene, backgroundMaterialPtr);
	auto* transformPtr = &scene->transformRoot;
	auto& m = scene->getCamera()->m;
	const mat4f rot = {
		vec4f{m[0][0], m[0][1], m[0][2], 0.0},
		vec4f{m[1][0], m[1][1], m[1][2], 0.0},
		vec4f{m[2][0], m[2][1], m[2][2], 0.0},
		vec4f{0.0, 0.0, 0.0, 1.0}
	};
	const auto& eye = scene->getCamera()->eye;
	static const auto BG_DISTANCE = 20.0;
	backgroundObjPtr->setTransform(
		transformPtr->createChild(mat4f::translate(eye*0.1))
		            ->createChild(rot)
		            ->createChild(mat4f::translate(vec3f{0.0, 0.0, -1.0} * BG_DISTANCE))
					->createChild(mat4f::scale(ONES * pow(BG_DISTANCE * 0.3, 2)))
	);
}

bool RayTracer::loadScene(char* fn)
{
	try
	{
		scene = readScene(fn);
	}
	catch (ParseError& pe)
	{
		fl_alert("ParseError: %s\n", pe.getMsg().c_str());
		return false;
	}

	if (!scene)
		return false;

	bufferWidth = 256;
	bufferHeight = lround(bufferWidth / scene->getCamera()->getAspectRatio());

	bufferSize = bufferWidth * bufferHeight * 3;
	buffer = new unsigned char[ bufferSize ];

	// Background
	{
		initializeBackgroundObj();
		auto* backgroundMaterialPtr = new Material();
		backgroundMaterialPtr->emissionTexturePtr = backgroundTexturePtr;
		backgroundObjPtr->setMaterial(backgroundMaterialPtr);

		const auto backgroundIt = std::find(scene->objects.begin(), scene->objects.end(), backgroundObjPtr);
		if (!isUsingBackground && backgroundIt != scene->objects.end())
		{
			// Remove the object from scene objects.
			scene->objects.erase(backgroundIt);
		}
		else if (isUsingBackground && backgroundIt == scene->objects.end())
		{
			// Add background object to scene.
			scene->add(backgroundObjPtr);
		}
	}
	// ---


	// separate objects into bounded and unbounded
	scene->initScene();

	// Add any specialized scene loading code here

	mBSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup(const int w, const int h, const int superSample)
{
	this->superSample = superSample;

	if (bufferWidth != w || bufferHeight != h)
	{
		bufferWidth = w;
		bufferHeight = h;

		bufferSize = bufferWidth * bufferHeight * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset(buffer, 0, w * h * 3);
}

void RayTracer::traceLines(const int start, int stop)
{
	if (!scene)
		return;

	if (stop > bufferHeight)
		stop = bufferHeight;

	for (auto j = start; j < stop; ++j)
		for (auto i = 0; i < bufferWidth; ++i)
			tracePixel(i, j);
}

vec3f RayTracer::adaptiveSampling(const double i, const double j, const double w, const double h, int depth)
{
	if (depth >= 3)return trace(scene, i, j);
	double x0 = i - w / 2.f, x1 = i + w / 2.f;
	double y0 = j - h / 2.f, y1 = j + h / 2.f;
	vec3f c00 = trace(scene, x0, y0),
	      c01 = trace(scene, x0, y1),
	      c10 = trace(scene, x1, y0),
	      c11 = trace(scene, x1, y1),
	      c = trace(scene, i, j);
	vec3f color = c;
	int total = 1;
	if ((c00 - c).length() > 0.1)
	{
		color += adaptiveSampling(i - w / 4.f, j - h / 4.f, w / 2.f, h / 2.f, depth + 1);
		++total;
	}
	if ((c01 - c).length() > 0.1)
	{
		color += adaptiveSampling(i - w / 4.f, j + h / 4.f, w / 2.f, h / 2.f, depth + 1);
		++total;
	}
	if ((c11 - c).length() > 0.1)
	{
		color += adaptiveSampling(i + w / 4.f, j + h / 4.f, w / 2.f, h / 2.f, depth + 1);
		++total;
	}
	if ((c10 - c).length() > 0.1)
	{
		color += adaptiveSampling(i + w / 4.f, j - h / 4.f, w / 2.f, h / 2.f, depth + 1);
		++total;
	}
	if (isAdaptiveIllustrate && total != 1)color = vec3f(0, 0, 1);
	return color / total;
}

void RayTracer::tracePixel(const int i, const int j)
{
	if (!scene)
		return;

	vec3f sum, col;
	const auto x = double(i) / double(bufferWidth);
	const auto y = double(j) / double(bufferHeight);

	if (!isAdaptiveSuper)
	{
		const auto widthInterval = 1.0 / bufferWidth / superSample;
		const auto heightInterval = 1.0 / bufferHeight / superSample;


		for (auto jj = 0; jj < superSample; jj++)
		{
			for (auto ii = 0; ii < superSample; ii++)
			{
				sum += trace(scene, x + ii * widthInterval, y + jj * heightInterval);
			}
		}

		col = sum / (superSample * superSample);
	}
	else
	{
		col = adaptiveSampling(x, y, 1.0 / bufferWidth, 1.0 / bufferHeight, 0);
	}


	auto* pixel = buffer + (i + j * bufferWidth) * 3;

	pixel[0] = int(255.0 * col[0]);
	pixel[1] = int(255.0 * col[1]);
	pixel[2] = int(255.0 * col[2]);
}
