// The main ray tracer.

#include <FL/fl_ask.H>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace(Scene* scene, double x, double y)
{
	Ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
	scene->getCamera()->rayThrough(x, y, r);
	return traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay(Scene* scene, const Ray& r,
                          const vec3f& thresh, int depth)
{
	ISect i;

	if (scene->intersect(r, i))
	{
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const auto& m = i.getMaterial();
		return m.shade(scene, r, i);
	}
	// No intersection.  This ray travels to infinity, so we color
	// it according to the background color, which in this (simple) case
	// is just black.

	return vec3f(0.0, 0.0, 0.0);
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

	// separate objects into bounded and unbounded
	scene->initScene();

	// Add any specialized scene loading code here

	mBSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup(int w, int h)
{
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

void RayTracer::tracePixel(int i, int j)
{
	if (!scene)
		return;

	const auto x = double(i) / double(bufferWidth);
	const auto y = double(j) / double(bufferHeight);

	auto col = trace(scene, x, y);

	auto* pixel = buffer + (i + j * bufferWidth) * 3;

	pixel[0] = int(255.0 * col[0]);
	pixel[1] = int(255.0 * col[1]);
	pixel[2] = int(255.0 * col[2]);
}
