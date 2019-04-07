#include "ray.h"
#include "material.h"
#include "scene.h"

const Material&
ISect::getMaterial() const
{
	return material ? *material : obj->getMaterial();
}
