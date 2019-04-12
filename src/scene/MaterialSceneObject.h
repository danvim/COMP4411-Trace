#pragma once
#include "material.h"
#include "SceneObject.h"

// A simple extension of SceneObject that adds an instance of Material
// for simple material bindings.
class MaterialSceneObject: public SceneObject
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