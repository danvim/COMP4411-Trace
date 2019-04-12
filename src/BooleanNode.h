#pragma once
#include "scene/scene.h"

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
};
