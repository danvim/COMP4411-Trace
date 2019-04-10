//
// scene.h
//
// The Scene class and the geometric types that it can contain.
//

#ifndef SCENE_H_
#define SCENE_H_

#include <list>

#include "material.h"
#include "camera.h"
#include "../vecmath/vecmath.h"
#include <map>

class Light;
class Scene;

class SceneElement
{
public:
	virtual ~SceneElement() = default;

	Scene* getScene() const { return scene; }

protected:
	explicit SceneElement(Scene* s)
		: scene(s)
	{
	}

	Scene* scene;
};

class BoundingBox
{
public:
	vec3f min;
	vec3f max;

	BoundingBox& operator=(const BoundingBox& target);

	// Does this bounding box intersect the target?
	bool intersects(const BoundingBox& target) const;

	// does the box contain this point?
	bool intersects(const vec3f& point) const;

	// if the ray hits the box, put the "t" value of the intersection
	// closest to the origin in tMin and the "t" value of the far intersection
	// in tMax and return true, else return false.
	bool intersect(const Ray& r, double& tMin, double& tMax) const;
};

class TransformNode
{
protected:

	// information about this node's transformation
	mat4f xform;
	mat4f inverse;
	mat3f normi;

	// information about parent & children
	TransformNode* parent;
	std::list<TransformNode*> children;

public:
	typedef std::list<TransformNode*>::iterator ChildIter;
	typedef std::list<TransformNode*>::const_iterator ChildCIter;

	~TransformNode()
	{
		for (auto c = children.begin(); c != children.end(); ++c)
			delete *c;
	}

	TransformNode* createChild(const mat4f& xform)
	{
		auto* child = new TransformNode(this, xform);
		children.push_back(child);
		return child;
	}

	// Coordinate-Space transformation
	vec3f globalToLocalCoords(const vec3f& v) const
	{
		return inverse * v;
	}

	vec3f localToGlobalCoords(const vec3f& v) const
	{
		return xform * v;
	}

	vec4f localToGlobalCoords(const vec4f& v) const
	{
		return xform * v;
	}

	vec3f localToGlobalCoordsNormal(const vec3f& v) const
	{
		return (normi * v).normalize();
	}

protected:
	// protected so that users can't directly construct one of these...
	// force them to use the createChild() method.  Note that they CAN
	// directly create a TransformRoot object.
	TransformNode(TransformNode* parent, const mat4f& xform)
	{
		this->parent = parent;
		if (parent == nullptr)
			this->xform = xform;
		else
			this->xform = parent->xform * xform;

		inverse = this->xform.inverse();
		normi = this->xform.upper33().inverse().transpose();
	}
};

class TransformRoot : public TransformNode
{
public:
	TransformRoot()
		: TransformNode(nullptr, mat4f())
	{
	}
};

// A Geometry object is anything that has extent in three dimensions.
// It may not be an actual visible scene object.  For example, hierarchical
// spatial subdivision could be expressed in terms of Geometry instances.
class Geometry
	: public SceneElement
{
public:
	// intersections performed in the global coordinate space.
	virtual bool intersect(const Ray& r, ISect& i) const;

	// intersections performed in the object's local coordinate space
	// do not call directly - this should only be called by intersect()
	virtual bool intersectLocal(const Ray& r, ISect& i) const;


	virtual bool hasBoundingBoxCapability() const;
	const BoundingBox& getBoundingBox() const { return bounds; }

	virtual void computeBoundingBox()
	{
		// take the object's local bounding box, transform all 8 points on it,
		// and use those to find a new bounding box.

		const auto localBounds = ComputeLocalBoundingBox();

		auto min = localBounds.min;
		auto max = localBounds.max;

		auto v = transform->localToGlobalCoords(vec4f(min[0], min[1], min[2], 1));
		auto newMax = v;
		auto newMin = v;
		v = transform->localToGlobalCoords(vec4f(max[0], min[1], min[2], 1));
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords(vec4f(min[0], max[1], min[2], 1));
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords(vec4f(max[0], max[1], min[2], 1));
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords(vec4f(min[0], min[1], max[2], 1));
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords(vec4f(max[0], min[1], max[2], 1));
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords(vec4f(min[0], max[1], max[2], 1));
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords(vec4f(max[0], max[1], max[2], 1));
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);

		bounds.max = vec3f(newMax);
		bounds.min = vec3f(newMin);
	}

	// default method for ComputeLocalBoundingBox returns a bogus bounding box;
	// this should be overridden if hasBoundingBoxCapability() is true.
	virtual BoundingBox ComputeLocalBoundingBox() const { return BoundingBox(); }

	void setTransform(TransformNode* transform) { this->transform = transform; };

	explicit Geometry(Scene* scene)
		: SceneElement(scene)
	{
	}

protected:
	BoundingBox bounds;
	TransformNode* transform{};
};

// A SceneObject is a real actual thing that we want to model in the 
// world.  It has extent (its Geometry heritage) and surface properties
// (its material binding).  The decision of how to store that material
// is left up to the subclass.
class SceneObject
	: public Geometry
{
public:
	virtual const Material& getMaterial() const = 0;
	virtual void setMaterial(Material* m) = 0;

protected:
	explicit SceneObject(Scene* scene)
		: Geometry(scene)
	{
	}
};

// A simple extension of SceneObject that adds an instance of Material
// for simple material bindings.
class MaterialSceneObject
	: public SceneObject
{
public:
	virtual ~MaterialSceneObject() { delete material; }

	const Material& getMaterial() const override { return *material; }
	void setMaterial(Material* m) override { material = m; }

	virtual std::pair<double, double> getUV(const Ray& r, const ISect& i) const { return {}; }

protected:
	MaterialSceneObject(Scene* scene, Material* mat)
		: SceneObject(scene), material(mat)
	{
	}

	//	MaterialSceneObject( Scene *scene ) 
	//	: SceneObject( scene ), material( new Material ) {}

	Material* material;
};

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

	void add(Geometry* obj)
	{
		obj->computeBoundingBox();
		objects.push_back(obj);
	}

	void add(Light* light)
	{
		lights.push_back(light);
	}

	bool intersect(const Ray& r, ISect& i) const;
	void initScene();

	std::list<Light*>::const_iterator beginLights() const { return lights.begin(); }
	std::list<Light*>::const_iterator endLights() const { return lights.end(); }

	Camera* getCamera() { return &camera; }

	double distAtteA = 0, distAtteB = 0, distAtteC = 0;
	bool softShadow = false, glossyReflection = false, motionBlur = false;
	double terminationThreshold = 0.9;
	vec3f ambientLight = vec3f(0, 0, 0);

	std::map<std::string, Texture*> texturePool;

private:
	std::list<Geometry*> objects;
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
