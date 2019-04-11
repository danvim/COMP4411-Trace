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
    vec3f traceRay(Scene* scene, const Ray& r, const vec3f& thresh, int depth, std::stack<Material> materials);
    vec3f refraction2(vec3f i, vec3f n, double n1, double n2);
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
	void traceSetup(int w, int h, const int superSample);
	void traceLines( int start = 0, int stop = 10000000 );
    vec3f adaptiveSampling(double i, double j, double w, double h, int depth);
    void tracePixel(int i, int j);

	bool loadScene( char* fn );

	bool sceneLoaded() const;

	Scene* getScene() const { return scene; }

    int maxDepth = 0;
	bool isAdaptiveSuper = false;
	bool isAdaptiveIllustrate = false;
	std::stack<double> refractiveIndex;
	std::stack<Material> materials;
	MaterialSceneObject* backgroundObjPtr = nullptr;
	bool isUsingBackground = false;
	Texture* backgroundTexturePtr = nullptr;

private:
	unsigned char *buffer;
	int bufferWidth, bufferHeight;
	int bufferSize{};
	int superSample{};
	Scene *scene;

	bool mBSceneLoaded;

	void initializeBackgroundObj();
};

#endif // RAY_TRACER_H_
