#include "Marble.h"
#include "PerlinNoise.h"

Marble::Marble(const vec3f& color, const double frequency, const double lightness, const double contrast,
               const vec3f& scale, const int depth):
	color(color),
	frequency(frequency),
	lightness(lightness),
	contrast(contrast),
	scale(scale),
	depth(depth)
{
}

vec3f Marble::getColor(const vec3f& position) const
{
	return (
			lightness +
			(1 - lightness) * sin(frequency * (position[0] + contrast * getTurbulence(position)))
		)
		* color;
}

double Marble::getTurbulence(const vec3f& position) const
{
	auto sum = 0.0;
	auto mag = 1.0;
	const auto p = prod(position, scale);

	for (auto i = 0; i < depth; i++)
	{
		sum += mag * PerlinNoise::noise(p[0], p[1], p[2]);
		mag /= 2;
	}

	return sum;
}
