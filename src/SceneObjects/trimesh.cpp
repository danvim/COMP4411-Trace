#include <cmath>
#include <cfloat>
#include "trimesh.h"

Trimesh::~Trimesh()
{
	for (auto i = materials.begin(); i != materials.end(); ++i)
	{
		delete *i;
	}
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex(const Eigen::Vector3d& v)
{
	vertices.push_back(v);
}

void Trimesh::addMaterial(Material* m)
{
	materials.push_back(m);
}

void Trimesh::addNormal(const Eigen::Vector3d& n)
{
	normals.push_back(n);
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace(const int a, const int b, const int c)
{
	const int vCount = vertices.size();

	if (a >= vCount || b >= vCount || c >= vCount)
		return false;

	auto* newFace = new TrimeshFace(scene, new Material(*this->material), this, a, b, c);
	newFace->setTransform(this->transform);
	faces.push_back(newFace);
	scene->add(newFace);
	return true;
}

std::string Trimesh::doubleCheck() const
// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
{
	if (!materials.empty() && materials.size() != vertices.size())
		return "Bad Trimesh: Wrong number of materials.";
	if (!normals.empty() && normals.size() != vertices.size())
		return "Bad Trimesh: Wrong number of normals.";

	return nullptr;
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in bary.
// Uses the algorithm and notation from _Graphic Gems 5_, p. 232.
//
// Calculates and returns the normal of the triangle too.
bool TrimeshFace::intersectLocal(const Ray& r, ISect& i) const
{
	const auto& a = parent->vertices[ids[0]];
	const auto& b = parent->vertices[ids[1]];
	const auto& c = parent->vertices[ids[2]];

	Eigen::Vector3d bary;

	const auto p = r.getPosition();
	auto v = r.getDirection();

	Eigen::Vector3d ab = b - a;
	const Eigen::Vector3d ac = c - a;
	Eigen::Vector3d ap = p - a;

	auto cv = ab.cross(ac);

	// there exists some bad triangles such that two vertices coincide
	// check this before normalize
	if (cv.isZero()) return false;
	auto n = cv.normalized();

	const auto vDotN = v.transpose().dot(n);
	if (-vDotN < NORMAL_EPSILON)
		return false;

	const auto t = - ap.transpose().dot(n) / vDotN;

	if (t < RAY_EPSILON)
		return false;

	// find k where k is the index of the component
	// of normal vector with greatest absolute value
	double greatestMag = FLT_MIN;
	auto k = -1;
	for (auto j = 0; j < 3; ++j)
	{
		auto val = n[j];
		if (val < 0)
			val *= -1;
		if (val > greatestMag)
		{
			k = j;
			greatestMag = val;
		}
	}

	Eigen::Vector3d am = ap + t * v;

	bary[1] = am.cross(ac)[k] / ab.cross(ac)[k];
	bary[2] = ab.cross(am)[k] / ab.cross(ac)[k];
	bary[0] = 1 - bary[1] - bary[2];
	if (bary[0] < 0 || bary[1] < 0 || bary[1] > 1 || bary[2] < 0 || bary[2] > 1)
		return false;

	// if we get this far, we have an intersection.  Fill in the info.
	i.setT(t);
	if (!parent->normals.empty())
	{
		// use interpolated normals
		i.setN((bary[0] * parent->normals[ids[0]]
			+ bary[1] * parent->normals[ids[1]]
			+ bary[2] * parent->normals[ids[2]]).normalized());
	}
	else
	{
		i.setN(n); // use face normal
	}
	i.obj = this;

	// linearly interpolate materials
	if (!parent->materials.empty())
	{
		auto* m = new Material();
		for (auto jj = 0; jj < 3; ++jj)
			*m += bary[jj] * *parent->materials[ids[jj]];
		i.setMaterial(m);
	}

	return true;
}

void Trimesh::generateNormals()
// Once you've loaded all the vertices and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
{
	const int count = vertices.size();
	normals.resize(count);
	auto* numFaces = new int[ count ]; // the number of faces assoc. with each vertex
	memset(numFaces, 0, sizeof(int) * count);

	for (auto fi = faces.begin(); fi != faces.end(); ++fi)
	{
		auto a = vertices[(**fi)[0]];
		auto b = vertices[(**fi)[1]];
		auto c = vertices[(**fi)[2]];

		const auto faceNormal = (b - a).cross(c - a).normalized();

		for (auto i = 0; i < 3; ++i)
		{
			normals[(**fi)[i]] += faceNormal;
			++numFaces[(**fi)[i]];
		}
	}

	for (auto i = 0; i < count; ++i)
	{
		if (numFaces[i])
			normals[i] /= numFaces[i];
	}

	delete [] numFaces;
}
