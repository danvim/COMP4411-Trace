//
// scene.h
//
// The Scene class and the geometric types that it can contain.
//

#ifndef SCENE_H_
#define SCENE_H_

#include <list>
#include <stack>
#include <map>
#include "../vecmath/vecmath.h"
#include "BoundingBox.h"
#include "camera.h"
#include "TransformNode.h"

class Light;
class Geometry;
class Ray;
class ISect;
class Texture;

class Scene
{
public:
	typedef std::list<Light*>::iterator LightIter;
	typedef std::list<Light*>::const_iterator CLightIter;

	typedef std::list<Geometry*>::iterator GeometryIter;
	typedef std::list<Geometry*>::const_iterator CGeometryIter;

	TransformRoot transformRoot;

	Scene() = default;

	virtual ~Scene();

	void add(Geometry* obj);

	void add(Light* light);
	void loadHeightMap(unsigned char* height_map_ptr, int w, int h);

	bool intersect(const Ray& r, ISect& i, std::stack<Geometry*>& intersections) const;
	void initScene();

	std::list<Light*>::const_iterator beginLights() const;
	std::list<Light*>::const_iterator endLights() const;

	Camera* getCamera();

	double distAtteA = 0, distAtteB = 0, distAtteC = 0;
	bool softShadow = false, glossyReflection = false, motionBlur = false, fod = false;
	double focalLength = 1, aperture = 1;
	double terminationThreshold = 0.9;
	vec3f ambientLight = vec3f(0, 0, 0);

	std::map<std::string, Texture*> texturePool;
	std::list<Geometry*> objects;

private:
	std::list<Geometry*> unboundedObjects;
	std::list<Geometry*> boundedObjects;
	std::list<Light*> lights;
	Camera camera;

	// Each object in the scene, provided that it has hasBoundingBoxCapability(),
	// must fall within this bounding box.  Objects that don't have hasBoundingBoxCapability()
	// are exempt from this requirement.
	BoundingBox sceneBounds;
};

#endif // SCENE_H_
