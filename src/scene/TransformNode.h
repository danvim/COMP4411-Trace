#pragma once
#include "../vecmath/vecmath.h"
#include <list>


class TransformNode
{
protected:

	// information about this node's transformation
	mat4f xform;
	mat4f inverse;
	mat3f normi;

public:
	// information about parent & children
	TransformNode* parent;
	std::list<TransformNode*> children;

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