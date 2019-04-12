#pragma once
#include "scene.h"

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
