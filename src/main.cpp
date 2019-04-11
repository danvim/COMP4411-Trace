//=============================================================================
// The following tree diagrams the execution path for the ray tracer (when it
// is run as a console application, the GUI complicates things a little).  Only
// the most salient features are shown here, the loadScene method, for example,
// has many, many sub-routines.  A good place to start with this project is a
// quick tour of these methods.  The ones that need to be completed by you are
// noted as such.  A quick explanation of the various methods follows the
// diagram.
//
// main
//  |
//  +- RayTracer::loadScene
//  |
//  +- RayTracer::traceSetup
//  |
//  +- RayTracer::traceLines
//        |
//        +- RayTracer::tracePixel
//              |
//              +- RayTracer::trace
//                    |
//                    +- Camera::rayThrough
//                    |
//                    +- RayTracer::traceRay
//                          |
//                          +- Scene::intersect
//                          |     |
//                          |     +- <Geometry>::intersect
//                          |           |
//                          |           +- <Geometry>::intersectLocal
//                          |
//                          +- isect::getMaterial
//                          |
//                          +- Material::shade
//
// The loadScene and traceSetup methods load a file and set up all the internal
// buffers necessary to render the scene.  The traceLines method begins the
// process of actually rendering the image, one scanline at a time.  It does
// this by calling tracePixel for each pixel in the image.  tracePixel is given
// a coordinate pair which is converted into an (x,y) screen coordinate and
// passed to trace.  The trace method calculates a ray from the camera position
// through the (x,y) coordinate and then calls traceRay to see if this ray
// actually intersects any objects in the scene.  The intersect method in
// Scene calls intersect on each object in the scene (part of your assignment
// is an acceleration or culling process that cuts this down significantly).
// Each object in the scene is a descendant of Geometry and has its own
// intersectLocal routine (you need to fill this method in for the Box class).
// The intersect method actually converts the ray into the coordinate frame
// of the object where intersectLocal can check for an intersection.
// Finally, if an intersection was found, the material for that object is
// obtained and used to shade the scene (you will need to provide the code to
// figure out the correct color for the shading).
//
//=============================================================================

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include <FL/fl_ask.H>

#include "ui/TraceUI.h"
#include "RayTracer.h"

#include "fileio/bitmap.h"
#include <sstream>
#include <vector>

#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif

double diff(double a, double b)
{
	return abs(a - b);
}

// ***********************************************************
// from getopt.cpp 
// it should be put in an include file.
//
extern int getopt(int argc, char** argv, char* optString);
extern char* optArg;
extern int optInd, optErr, optOpt;
// ***********************************************************

RayTracer* theRayTracer;
TraceUi* traceUi;

std::stringstream ss;
//
// options from program parameters
//
int recursionDepth = 0;
int gHeight;
int gWidth = 150;
bool bReport = false;
char *progName, *rayName, *imgName;

void usage()
{
#ifdef WIN32
	fl_alert("usage: %s [-r <#> -w <#> -t] [input.ray output.bmp]\n", progName);
#else
	fprintf( stderr, "usage: %s [options] [input.ray output.bmp]\n", progName );
	fprintf( stderr, "  -r <#>      set recurssion level (default %d)\n", recursion_depth );
	fprintf( stderr, "  -w <#>      set output image width (default %d)\n", g_width );
	fprintf( stderr, "  -t			report time statistics\n" );
#endif
}

bool processArgs(int argc, char** argv)
{
	int i;

	while ((i = getopt(argc, argv, "tr:w:h:")) != EOF)
	{
		switch (i)
		{
		case 't':
			bReport = true;
			break;

		case 'r':
			recursionDepth = atoi(optArg);
			break;

		case 'w':
			gWidth = atoi(optArg);
			break;

		case 'h':
			gHeight = atoi(optArg);
			break;

		default:
			return false;
		}
	}

	if (optInd >= argc - 1)
	{
		fprintf(stderr, "no input and/or output name.\n");
		return false;
	}

	rayName = argv[optInd];
	imgName = argv[optInd + 1];

	return true;
}

float frand()
{
	return (float)rand() / RAND_MAX;
}

double drand(double low, double high)
{
	return (double)rand() / RAND_MAX * (high-low) + low;
}

int irand(int max)
{
	return rand() % max;
}

double degToRad(double deg)
{
	return deg * 2 * M_PI / 360.0f;
}

double radToDeg(double rad)
{
	return rad / 2.f / M_PI * 360.0f;
}

std::vector<vec3f> sampleDistributed(vec3f c, double r, int count)
{
	vec3f up = vec3f(0, 1, 0);
	if((c.normalize()-up).length() < RAY_EPSILON)
	{
		up = vec3f(0, 0, 1);
	}
	vec3f u = (c.cross(up)).normalize();
	vec3f v = (u.cross(c)).normalize();
	u = (c.cross(v)).normalize();

	// double d1 = c.dot(u), d2 = c.dot(v), d3 = u.dot(v);
	// if(d1>RAY_EPSILON||d2 > RAY_EPSILON ||d3 > RAY_EPSILON)
	// {
	// 	std::cout << d1 << d2 << d3<<u<<v<<up;
	// }

	std::vector<vec3f> vecs;
	for(int i=0;i<count;i++)
	{
		double x = frand() * 2 * r - r;
		double y = frand() * 2 * r - r;
		vec3f t = c + x*u + y*v;
		t = t.normalize();
		vecs.push_back(t);
	}
	return vecs;
}

// usage : ray [option] in.ray out.bmp
// Simply keying in ray will invoke a graphics mode version.
// Use "ray --help" to see the detailed usage.
// OK. I am lying. any illegal option such as "ray blahbalh" will print
// out the usage
//
// Graphics mode will be substantially slower than text mode because of
// event handling overhead.
int main(int argc, char** argv)
{
	// RayTracer rt;
	// sampleDistributed({ 1,0,0 }, 0.05, 1);
	// ss << rt.refraction(vec3f(cos(M_PI / 6), sin(M_PI / 6), 0), vec3f(0, 1, 0), 1, 1.33).normalize() << std::endl;
	// ss << rt.refraction(vec3f(cos(M_PI / 6), sin(M_PI / 6), 0), vec3f(0, 1, 0), 1.33, 1).normalize() << std::endl;
	// ss << rt.refraction(vec3f(cos(M_PI / 3), sin(M_PI / 3), 0), vec3f(0, 1, 0), 1, 1.33).normalize() << std::endl;
	// ss << rt.refraction(vec3f(cos(M_PI / 3), sin(M_PI / 3), 0), vec3f(0, 1, 0),  1.33, 1).normalize() << std::endl;
	// OutputDebugString(ss.str().c_str());
	// return 0;

	progName = argv[0];

	if (argc != 1)
	{
		// text mode
		if (!processArgs(argc, argv))
		{
			usage();
			exit(1);
		}

		theRayTracer = new RayTracer();
		theRayTracer->loadScene(rayName);

		if (theRayTracer->sceneLoaded())
		{
			gHeight = int(gWidth / theRayTracer->aspectRatio() + 0.5);

			theRayTracer->traceSetup(gWidth, gHeight, 0);

			clock_t start = clock();

			theRayTracer->traceLines(0, gHeight);

			clock_t end = clock();

			// save image
			unsigned char* buf;

			theRayTracer->getBuffer(buf, gWidth, gHeight);
			if (buf)
				writeBmp(imgName, gWidth, gHeight, buf);

			if (bReport)
			{
				double t = double(end - start) / CLOCKS_PER_SEC;
#ifdef WIN32
				fl_message("total time = %.3f seconds\n", t);
#else
				fprintf( stderr, "total time = %.3f seconds\n", t); 
#endif
			}
		}

		return 1;
	}
	// graphics mode
	traceUi = new TraceUi();
	theRayTracer = new RayTracer();

	traceUi->setRayTracer(theRayTracer);

	Fl::visual(FL_DOUBLE | FL_INDEX);

	traceUi->show();

	return Fl::run();
}
