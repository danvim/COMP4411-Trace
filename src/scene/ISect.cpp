#include "ISect.h"

ISect::ISect(): obj(nullptr), t(0.0), material(nullptr)
{
}

ISect::~ISect()
{
	delete material;
}

void ISect::setObject(SceneObject* o)
{
	obj = o;
}

void ISect::setT(const double tt)
{
	t = tt;
}

void ISect::setN(const vec3f& n)
{
	N = n;
}

void ISect::setMaterial(Material* m)
{
	delete material;
	material = m;
}

ISect& ISect::operator=(const ISect& other)
{
	if (this != &other)
	{
		obj = other.obj;
		t = other.t;
		N = other.N;
		//            material = other.material ? new Material( *(other.material) ) : 0;
		if (other.material)
		{
			if (material)
				*material = *other.material;
			else
				material = new Material(*other.material);
		}
		else
		{
			material = nullptr;
		}
	}
	return *this;
}

const Material &
ISect::getMaterial() const
{
	return material ? *material : obj->getMaterial();
}
