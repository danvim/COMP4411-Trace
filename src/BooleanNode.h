#pragma once
#include "scene/SceneObject.h"

class BooleanNode: public SceneObject
{
public:
	BooleanNode(Scene* const scene, SceneObject* const a, SceneObject* const b)
		: SceneObject(scene),
		  a(a),
		  b(b)
	{
	}

	virtual ~BooleanNode();

	const Material& getMaterial() const override;
	void setMaterial(Material* m) override;

	SceneObject* a{};
	SceneObject* b{};

	Ray getLocalRay(const Ray& r) const;;
};
