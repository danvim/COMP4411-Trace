#pragma once
#include "../light.h"

class SpotLight final : public PointLight
{
public:
	SpotLight(Scene* scene, const vec3f& color, const vec3f& position, const vec3f& orientation, double size, double blend);

	vec3f shadowAttenuation(const vec3f& p, std::stack<Geometry*>& intersections) const override;

protected:
	vec3f orientation;
	/**
	 * Angle of the spot light in radian.
	 */
	double size;

	/**
	 * Percentage of linearly blending radius. 0~1
	 */
	double blend;

	double cosOuterCone;
	double cosInnerCone;

	/**
	 * Returns the angle of the inner cone, behind the blending region.
	 * @code
	 *       *`*
	 *      *.|.*
	 *     * *|+ *
	 *    * . |+. *
	 *   *  * |++--+--- inner cone  
	 *  *  .  |++.  *
	 * *   *  |+++   *
	 * @endcode
	 */
	double innerCone() const;
};
