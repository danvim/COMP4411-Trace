#pragma once
#include "Geometry.h"
#include "material.h"

// A SceneObject is a real actual thing that we want to model in the 
// world.  It has extent (its Geometry heritage) and surface properties
// (its material binding).  The decision of how to store that material
// is left up to the subclass.
class SceneObject: public Geometry
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