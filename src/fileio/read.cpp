#ifdef WIN32
#pragma warning( disable : 4786 )
#endif

#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>

#include <vector>

#include "read.h"
#include "parse.h"

#include "../scene/scene.h"
#include "../SceneObjects/trimesh.h"
#include "../SceneObjects/Box.h"
#include "../SceneObjects/Cone.h"
#include "../SceneObjects/Cylinder.h"
#include "../SceneObjects/Sphere.h"
#include "../SceneObjects/Square.h"
#include "../scene/light.h"

typedef std::map<std::string, Material*> MMap;

static void processObject(Obj* obj, Scene* scene, MMap& materials);
static Obj* getColorField(Obj* obj);
static Obj* getField(Obj* obj, const std::string& name);
static bool hasField(Obj* obj, const std::string& name);
static vec3f tupleToVec(Obj* obj);
static void processGeometry(const std::string& name, Obj* child, Scene* scene,
                            const MMap& materials, TransformNode* transform);
static void processTrimesh(std::string name, Obj* child, Scene* scene,
                           const MMap& materials, TransformNode* transform);
static void processCamera(Obj* child, Scene* scene);
static Material* getMaterial(Obj* child, const MMap& bindings);
static Material* processMaterial(Obj* child, MMap* bindings = nullptr);
static void verifyTuple(const MyTuple& tup, size_t size);

Scene* readScene(const std::string& filename)
{
	std::ifstream ifs(filename.c_str());
	if (!ifs)
	{
		std::cerr << "Error: couldn't read scene file " << filename << std::endl;
		return nullptr;
	}

	try
	{
		return readScene(ifs);
	}
	catch (ParseError& pe)
	{
		std::cout << "Parse error: " << pe << std::endl;
		return nullptr;
	}
}

Scene* readScene(std::istream& is)
{
	auto* ret = new Scene;

	// Extract the file header
	static const auto MAX_NAME = 80;
	char buf[ MAX_NAME ];
	auto ct = 0;

	while (ct < MAX_NAME - 1)
	{
		char c;
		is.get(c);
		if (c == ' ' || c == '\t' || c == '\n')
		{
			break;
		}
		buf[ct++] = c;
	}

	buf[ct] = '\0';

	if (strcmp(buf, "SBT-raytracer") != 0)
	{
		throw ParseError(std::string("Input is not an SBT input file."));
	}

	float version;
	is >> version;

	if (version != 1.0)
	{
		std::ostringstream oss;
		oss << "Input is version " << version << ", need version 1.0" << std::ends;

		throw ParseError(std::string(oss.str()));
	}

	// vector<Obj*> result;
	MMap materials;

	while (true)
	{
		const auto cur = readFile(is);
		if (!cur)
		{
			break;
		}

		processObject(cur, ret, materials);
		delete cur;
	}

	return ret;
}

// Find a color field inside some object.  Now, I recognize that not
// everyone speaks the Queen's English, so I allow both spellings of
// color.  If you're composing scenes, you don't need to worry about
// spelling :^)
static Obj* getColorField(Obj* obj)
{
	if (obj == nullptr)
	{
		throw ParseError(std::string("Object contains no color field"));
	}

	const auto& d = obj->getDict();
	Dict::const_iterator i;
	if ((i = d.find("color")) != d.end())
	{
		return (*i).second;
	}
	if ((i = d.find("colour")) != d.end())
	{
		return (*i).second;
	}

	throw ParseError(std::string("Object contains no color field"));
}

// Get a named field out of a dictionary object.  Throw a parse
// error if the field isn't there.
static Obj* getField(Obj* obj, const std::string& name)
{
	if (obj == nullptr)
	{
		throw ParseError(std::string("Object contains no field named \"") +
			name + "\"");
	}

	const auto& d = obj->getDict();
	Dict::const_iterator i;
	if ((i = d.find(name)) != d.end())
	{
		return (*i).second;
	}

	throw ParseError(std::string("Object contains no field named \"") + name + "\"");
}

// Determine if the given dictionary object has the named field.
static bool hasField(Obj* obj, const std::string& name)
{
	if (obj == nullptr)
	{
		return false;
	}

	const auto& d = obj->getDict();
	return d.find(name) != d.end();
}

// Turn a parsed tuple into a 3D point.
static vec3f tupleToVec(Obj* obj)
{
	const auto& t = obj->getTuple();
	verifyTuple(t, 3);
	return vec3f(t[0]->getScalar(), t[1]->getScalar(), t[2]->getScalar());
}

static void processGeometry(Obj* obj, Scene* scene,
                            const MMap& materials, TransformNode* transform)
{
	std::string name;
	Obj* child;

	if (obj->getTypeName() == "id")
	{
		name = obj->getId();
		child = nullptr;
	}
	else if (obj->getTypeName() == "named")
	{
		name = obj->getName();
		child = obj->getChild();
	}
	else
	{
		std::ostringstream oss;
		oss << "Unknown input object ";
		obj->printOn(oss);

		throw ParseError(std::string(oss.str()));
	}

	processGeometry(name, child, scene, materials, transform);
}

// Extract the named scalar field into ret, if it exists.
static bool maybeExtractField(Obj* child, const std::string& name, double& ret)
{
	if (hasField(child, name))
	{
		ret = getField(child, name)->getScalar();
		return true;
	}

	return false;
}

// Extract the named boolean field into ret, if it exists.
static bool maybeExtractField(Obj* child, const std::string& name, bool& ret)
{
	if (hasField(child, name))
	{
		ret = getField(child, name)->getBoolean();
		return true;
	}

	return false;
}

// Check that a tuple has the expected size.
static void verifyTuple(const MyTuple& tup, size_t size)
{
	if (tup.size() != size)
	{
		std::ostringstream oss;
		oss << "Bad tuple size " << tup.size() << ", expected " << size;

		throw ParseError(std::string(oss.str()));
	}
}

static void processGeometry(const std::string& name, Obj* child, Scene* scene,
                            const MMap& materials, TransformNode* transform)
{
	if (name == "translate")
	{
		const auto& tup = child->getTuple();
		verifyTuple(tup, 4);
		processGeometry(tup[3],
		                scene,
		                materials,
		                transform->createChild(mat4f::translate(vec3f(tup[0]->getScalar(),
		                                                              tup[1]->getScalar(),
		                                                              tup[2]->getScalar()))));
	}
	else if (name == "rotate")
	{
		const auto& tup = child->getTuple();
		verifyTuple(tup, 5);
		processGeometry(tup[4],
		                scene,
		                materials,
		                transform->createChild(mat4f::rotate(vec3f(tup[0]->getScalar(),
		                                                           tup[1]->getScalar(),
		                                                           tup[2]->getScalar()),
		                                                     tup[3]->getScalar())));
	}
	else if (name == "scale")
	{
		const auto& tup = child->getTuple();
		if (tup.size() == 2)
		{
			auto sc = tup[0]->getScalar();
			processGeometry(tup[1],
			                scene,
			                materials,
			                transform->createChild(mat4f::scale(vec3f(sc, sc, sc))));
		}
		else
		{
			verifyTuple(tup, 4);
			processGeometry(tup[3],
			                scene,
			                materials,
			                transform->createChild(mat4f::scale(vec3f(tup[0]->getScalar(),
			                                                          tup[1]->getScalar(),
			                                                          tup[2]->getScalar()))));
		}
	}
	else if (name == "transform")
	{
		const auto& tup = child->getTuple();
		verifyTuple(tup, 5);

		const auto& l1 = tup[0]->getTuple();
		const auto& l2 = tup[1]->getTuple();
		const auto& l3 = tup[2]->getTuple();
		const auto& l4 = tup[3]->getTuple();
		verifyTuple(l1, 4);
		verifyTuple(l2, 4);
		verifyTuple(l3, 4);
		verifyTuple(l4, 4);

		processGeometry(tup[4],
		                scene,
		                materials,
		                transform->createChild(mat4f(vec4f(l1[0]->getScalar(),
		                                                   l1[1]->getScalar(),
		                                                   l1[2]->getScalar(),
		                                                   l1[3]->getScalar()),
		                                             vec4f(l2[0]->getScalar(),
		                                                   l2[1]->getScalar(),
		                                                   l2[2]->getScalar(),
		                                                   l2[3]->getScalar()),
		                                             vec4f(l3[0]->getScalar(),
		                                                   l3[1]->getScalar(),
		                                                   l3[2]->getScalar(),
		                                                   l3[3]->getScalar()),
		                                             vec4f(l4[0]->getScalar(),
		                                                   l4[1]->getScalar(),
		                                                   l4[2]->getScalar(),
		                                                   l4[3]->getScalar()))));
	}
	else if (name == "trimesh" || name == "polymesh")
	{
		// 'polymesh' is for backwards compatibility
		processTrimesh(name, child, scene, materials, transform);
	}
	else
	{
		SceneObject* obj = nullptr;

		//if( hasField( child, "material" ) )
		auto mat = getMaterial(getField(child, "material"), materials);
		//else
		//    mat = new Material();

		if (name == "sphere")
		{
			obj = new Sphere(scene, mat);
		}
		else if (name == "box")
		{
			obj = new Box(scene, mat);
		}
		else if (name == "cylinder")
		{
			obj = new Cylinder(scene, mat);
		}
		else if (name == "cone")
		{
			auto height = 1.0;
			auto bottom_radius = 1.0;
			auto top_radius = 0.0;
			auto capped = true;

			maybeExtractField(child, "height", height);
			maybeExtractField(child, "bottom_radius", bottom_radius);
			maybeExtractField(child, "top_radius", top_radius);
			maybeExtractField(child, "capped", capped);

			obj = new Cone(scene, mat, height, bottom_radius, top_radius, capped);
		}
		else if (name == "square")
		{
			obj = new Square(scene, mat);
		}

		obj->setTransform(transform);
		scene->add(obj);
	}
}

static void processTrimesh(std::string name, Obj* child, Scene* scene,
                           const MMap& materials, TransformNode* transform)
{
	Material* mat;

	if (hasField(child, "material"))
		mat = getMaterial(getField(child, "material"), materials);
	else
		mat = new Material();

	auto* tmesh = new Trimesh(scene, mat, transform);

	const auto& points = getField(child, "points")->getTuple();
	for (auto pi = points.begin(); pi != points.end(); ++pi)
		tmesh->addVertex(tupleToVec(*pi));

	const auto& faces = getField(child, "faces")->getTuple();
	for (auto fi = faces.begin(); fi != faces.end(); ++fi)
	{
		const auto& pointIds = (*fi)->getTuple();

		// triangulate here and now.  assume the poly is
		// concave and we can triangulate using an arbitrary fan
		if (pointIds.size() < 3)
			throw ParseError("Faces must have at least 3 vertices.");

		auto i = pointIds.begin();
		const auto a = int((*i++)->getScalar());
		auto b = int((*i++)->getScalar());
		while (i != pointIds.end())
		{
			const auto c = int((*i++)->getScalar());
			if (!tmesh->addFace(a, b, c))
				throw ParseError("Bad face in trimesh.");
			b = c;
		}
	}

	auto generateNormals = false;
	maybeExtractField(child, "gennormals", generateNormals);
	if (generateNormals)
		tmesh->generateNormals();

	if (hasField(child, "materials"))
	{
		const auto& mats = getField(child, "materials")->getTuple();
		for (auto mi = mats.begin(); mi != mats.end(); ++mi)
			tmesh->addMaterial(getMaterial(*mi, materials));
	}
	if (hasField(child, "normals"))
	{
		const auto& norms = getField(child, "normals")->getTuple();
		for (auto ni = norms.begin(); ni != norms.end(); ++ni)
			tmesh->addNormal(tupleToVec(*ni));
	}

	std::string error;
	if (!(error = tmesh->doubleCheck()).empty())
		throw ParseError(error.c_str());

	scene->add(tmesh);
}

static Material* getMaterial(Obj* child, const MMap& bindings)
{
	const auto tField = child->getTypeName();
	if (tField == "id")
	{
		auto i = bindings.find(child->getId());
		if (i != bindings.end())
		{
			return (*i).second;
		}
	}
	else if (tField == "std::string")
	{
		auto i = bindings.find(child->getString());
		if (i != bindings.end())
		{
			return (*i).second;
		}
	}
	// Don't allow binding.
	return processMaterial(child);
}

static Material* processMaterial(Obj* child, MMap* bindings)
// Generate a material from a parse sub-tree
//
// child   - root of parse tree
// MMap    - bindings of names to materials (if non-null)
// defmat  - material to start with (if non-null)
{
	auto* mat = new Material();

	if (hasField(child, "emissive"))
	{
		mat->ke = tupleToVec(getField(child, "emissive"));
	}
	if (hasField(child, "ambient"))
	{
		mat->ka = tupleToVec(getField(child, "ambient"));
	}
	if (hasField(child, "specular"))
	{
		mat->ks = tupleToVec(getField(child, "specular"));
	}
	if (hasField(child, "diffuse"))
	{
		mat->kd = tupleToVec(getField(child, "diffuse"));
	}
	if (hasField(child, "reflective"))
	{
		mat->kr = tupleToVec(getField(child, "reflective"));
	}
	else
	{
		mat->kr = mat->ks; // defaults to ks if none given.
	}
	if (hasField(child, "transmissive"))
	{
		mat->kt = tupleToVec(getField(child, "transmissive"));
	}
	if (hasField(child, "index"))
	{
		// index of refraction
		mat->index = getField(child, "index")->getScalar();
	}
	if (hasField(child, "shininess"))
	{
		mat->shininess = getField(child, "shininess")->getScalar();
	}

	if (bindings != nullptr)
	{
		// Want to bind, better have "name" field:
		if (hasField(child, "name"))
		{
			auto* field = getField(child, "name");
			const auto tField = field->getTypeName();
			std::string name;
			if (tField == "id")
			{
				name = field->getId();
			}
			else
			{
				name = field->getString();
			}

			(*bindings)[name] = mat;
		}
		else
		{
			throw ParseError(
				std::string("Attempt to bind material with no name"));
		}
	}

	return mat;
}

static void
processCamera(Obj* child, Scene* scene)
{
	if (hasField(child, "position"))
		scene->getCamera()->setEye(tupleToVec(getField(child, "position")));
	if (hasField(child, "quaternion"))
	{
		const auto& quaternion = getField(child, "quaternion")->getTuple();
		if (quaternion.size() != 4)
			throw ParseError("Bogus quaternion.");
		scene->getCamera()->setLook(quaternion[0]->getScalar(),
		                            quaternion[1]->getScalar(),
		                            quaternion[2]->getScalar(),
		                            quaternion[3]->getScalar());
	}
	if (hasField(child, "fov"))
		scene->getCamera()->setFov(getField(child, "fov")->getScalar());
	if (hasField(child, "aspectratio"))
		scene->getCamera()->setAspectRatio(getField(child, "aspectratio")->getScalar());
	if (hasField(child, "viewdir") && hasField(child, "updir"))
	{
		scene->getCamera()->setLook(tupleToVec(getField(child, "viewdir")).normalize(),
		                            tupleToVec(getField(child, "updir")).normalize());
	}
}

static void processObject(Obj* obj, Scene* scene, MMap& materials)
{
	// Assume the object is named.
	std::string name;
	Obj* child;

	if (obj->getTypeName() == "id")
	{
		name = obj->getId();
		child = nullptr;
	}
	else if (obj->getTypeName() == "named")
	{
		name = obj->getName();
		child = obj->getChild();
	}
	else
	{
		std::ostringstream oss;
		oss << "Unknown input object ";
		obj->printOn(oss);

		throw ParseError(std::string(oss.str()));
	}

	if (name == "directional_light")
	{
		if (child == nullptr)
		{
			throw ParseError("No info for directional_light");
		}

		scene->add(new DirectionalLight(scene,
		                                tupleToVec(getField(child, "direction")).normalize(),
		                                tupleToVec(getColorField(child))));
	}
	else if (name == "point_light")
	{
		if (child == nullptr)
		{
			throw ParseError("No info for point_light");
		}

		scene->add(new PointLight(scene,
		                          tupleToVec(getField(child, "position")),
		                          tupleToVec(getColorField(child))));
	}
	else if (name == "sphere" ||
		name == "box" ||
		name == "cylinder" ||
		name == "cone" ||
		name == "square" ||
		name == "translate" ||
		name == "rotate" ||
		name == "scale" ||
		name == "transform" ||
		name == "trimesh" ||
		name == "polymesh")
	{
		// polymesh is for backwards compatibility.
		processGeometry(name, child, scene, materials, &scene->transformRoot);
		//scene->add( geo );
	}
	else if (name == "material")
	{
		processMaterial(child, &materials);
	}
	else if (name == "camera")
	{
		processCamera(child, scene);
	}
	else
	{
		throw ParseError(std::string("Unrecognized object: ") + name);
	}
}
