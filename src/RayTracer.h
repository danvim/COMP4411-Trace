#ifndef RAY_TRACER_H_
#define RAY_TRACER_H_

// The main ray tracer.

#include "scene/scene.h"

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const Ray& r, const vec3f& thresh, int depth );


	void getBuffer( unsigned char *&buf, int &w, int &h ) const;
	double aspectRatio() const;
	void traceSetup(int w, int h, const int superSample);
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel(int i, int j);

	bool loadScene( char* fn );

	bool sceneLoaded() const;

	Scene* getScene() const { return scene; }

	int maxDepth = 0;

private:
	unsigned char *buffer;
	int bufferWidth, bufferHeight;
	int bufferSize;
	int superSample;
	Scene *scene;

	bool mBSceneLoaded;
};

#endif // RAY_TRACER_H_
