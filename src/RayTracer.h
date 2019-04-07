#ifndef RAY_TRACER_H_
#define RAY_TRACER_H_

// The main ray tracer.

#include "scene/scene.h"
#include "Eigen/Dense"

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    Eigen::Vector3d trace( Scene *scene, double x, double y );
	Eigen::Vector3d traceRay( Scene *scene, const Ray& r, const Eigen::Vector3d& thresh, int depth );


	void getBuffer( unsigned char *&buf, int &w, int &h ) const;
	double aspectRatio() const;
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );

	bool sceneLoaded() const;

private:
	unsigned char *buffer;
	int bufferWidth, bufferHeight;
	int bufferSize{};
	Scene *scene;

	bool mBSceneLoaded;
};

#endif // RAY_TRACER_H_
