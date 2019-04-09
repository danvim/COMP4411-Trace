#ifndef RAY_TRACER_H_
#define RAY_TRACER_H_

// The main ray tracer.

#include "scene/scene.h"
#include <stack>

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const Ray& r, const vec3f& thresh, int depth);
    vec3f refractionDirection(vec3f& normal, vec3f& dir, double indexFrom, double indexTo);
    /**
	 * i incidence vector
	 * n normal vector
	 * n1 source refraction index
	 * n2 target refraction index
	 * return t the refraction vector
	 */
	vec3f refraction(vec3f i, vec3f n, double n1, double n2);

	void getBuffer( unsigned char *&buf, int &w, int &h ) const;
	double aspectRatio() const;
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );

	bool sceneLoaded() const;

	Scene* getScene() { return scene; }

	int maxDepth = 0;
	std::stack<double> refractiveIndex;
	std::stack<Material> materials;

private:
	unsigned char *buffer;
	int bufferWidth, bufferHeight;
	int bufferSize{};
	Scene *scene;

	bool mBSceneLoaded;
};

#endif // RAY_TRACER_H_
