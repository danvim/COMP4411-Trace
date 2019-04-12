#include <cmath>
#include "scene.h"
#include "../ui/TraceUI.h"
#include "../SceneObjects/trimesh.h"
#include "ISect.h"
#include "light.h"

extern TraceUi* traceUi;


Scene::~Scene()
{
    GeometryIter g;

    for( g = objects.begin(); g != objects.end(); ++g ) {
		delete *g;
	}

	for( g = boundedObjects.begin(); g != boundedObjects.end(); ++g ) {
		delete *g;
	}

	for( g = unboundedObjects.begin(); g != boundedObjects.end(); ++g ) {
		delete *g;
	}

	for (auto& light : lights)
	{
		delete light;
	}
}

void Scene::add(Geometry* obj)
{
	obj->computeBoundingBox();
	objects.push_back(obj);
}

void Scene::add(Light* light)
{
	lights.push_back(light);
}

void Scene::loadHeightMap(unsigned char* height_map_ptr, int w, int h)
{
	Material *mat = new Material;
	TransformRoot *transform = new TransformRoot;
	Trimesh* mesh = new Trimesh(this, mat, transform);
	mat->kd = vec3f(0.5, 0.5, 0.5);
	// Trimesh* m2 = new Trimesh(this,mat,transform);
	// m2->addVertex({ 0,0,0 });
	// m2->addVertex({ 5,5,0 });
	// m2->addVertex({ 0,5,0 });
	// m2->addFace(0, 1, 2);
	// for(auto& f:m2->faces)
	// boundedObjects.push_back(f);
	// Sphere* sp = new Sphere(this, mat);
	// sp->setTransform(transform);
	// add(sp);
	// unboundedObjects.push_back(sp);
	// unboundedObjects.push_back(m2);

	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			double x = (1.0* i) / w;
			double y = (1.0*j) / h;
			double z = height_map_ptr[(i+j*w) * 3] / 255.f;
			mesh->addVertex({ x*5,z,y*5 });
			mesh->addMaterial(mat);
		}
	}
	for (int j = 0; j < h - 1; j++)
	{
		for (int i = 0; i < w - 1; i++)
		{
			mesh->addFace(j*w + i, (j + 1)*w + i, j*w + i + 1);
			mesh->addFace((j + 1)*w + i, (j + 1)*w + i + 1, j*w + i + 1);
		}
	}
	mesh->generateNormals();
	for(auto& f:mesh->faces)
	boundedObjects.push_back(f);
}

// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect(const Ray& r, ISect& i, std::stack<Geometry*>& intersections) const
{
	typedef std::list<Geometry*>::const_iterator iter;
	iter j;

	ISect cur;
	bool have_one = false;

	// try the non-bounded objects
	for( j = unboundedObjects.begin(); j != unboundedObjects.end(); ++j ) {
		if( (*j)->intersect( r, cur, intersections) ) {
			if( !have_one || cur.t < i.t ) {
				i = cur;
				have_one = true;
			}
		}
	}

	// try the bounded objects
	for( j = boundedObjects.begin(); j != boundedObjects.end(); ++j ) {
		if( (*j)->intersect( r, cur, intersections) ) {
			if( !have_one || cur.t < i.t ) {
				i = cur;
				have_one = true;
			}
		}
	}


	return have_one;
}

void Scene::initScene()
{
	bool first_boundedobject = true;

	typedef std::list<Geometry*>::const_iterator iter;
	// split the objects into two categories: bounded and non-bounded
	for( iter j = objects.begin(); j != objects.end(); ++j ) {
		if( (*j)->hasBoundingBoxCapability() )
		{
			boundedObjects.push_back(*j);

			// widen the scene's bounding box, if necessary
			if (first_boundedobject) {
				sceneBounds = (*j)->getBoundingBox();
				first_boundedobject = false;
			}
			else
			{
				BoundingBox b = (*j)->getBoundingBox();
				sceneBounds.max = maximum(sceneBounds.max, b.max);
				sceneBounds.min = minimum(sceneBounds.min, b.min);
			}
		}
		else
			unboundedObjects.push_back(*j);
	}
}

std::list<Light*>::const_iterator Scene::beginLights() const
{
	return lights.begin();
}

std::list<Light*>::const_iterator Scene::endLights() const
{
	return lights.end();
}

Camera* Scene::getCamera()
{
	return &camera;
}
