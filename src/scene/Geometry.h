#pragma once
#include "SceneElement.h"
#include "TransformNode.h"
#include "BoundingBox.h"

// A Geometry object is anything that has extent in three dimensions.
// It may not be an actual visible scene object.  For example, hierarchical
// spatial subdivision could be expressed in terms of Geometry instances.
class Geometry
	: public SceneElement
{
public:
	// intersections performed in the global coordinate space.
	virtual bool intersect(const Ray& r, ISect& i, std::stack<Geometry*>& intersections) const;

	// intersections performed in the object's local coordinate space
	// do not call directly - this should only be called by intersect()
	virtual bool intersectLocal(const Ray& r, ISect& i) const;


	virtual bool hasBoundingBoxCapability() const;
	const BoundingBox& getBoundingBox() const { return bounds; }

	virtual void computeBoundingBox();

	// default method for computeLocalBoundingBox returns a bogus bounding box;
	// this should be overridden if hasBoundingBoxCapability() is true.
	virtual BoundingBox computeLocalBoundingBox() const { return BoundingBox(); }

	/**
	 * Checks if the position is within the mesh. Used for CSG
	 */
	virtual bool contains(bool intersections) const { return true; }

	virtual bool isFullyEnclosed() const { return true; }

	void setTransform(TransformNode* transform) { this->transform = transform; };

	explicit Geometry(Scene* scene);

protected:
	BoundingBox bounds;
	TransformNode* transform{};
};
