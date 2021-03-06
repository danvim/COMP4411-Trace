﻿#include "Texture.h"
#include "../fileio/bitmap.h"

vec3f Texture::getBoxColor(double u, double v)
{
	return vec3f{};
}

/**
 * u and v ranges from 0.0 to 1.0
 * (0,0) marks 1 corner and (1,1) marks the opposite color.
 */
vec3f Texture::getSquareColor(const double u, const double v) const
{
	const auto pixel = imagePtr + (int(v * imageHeight) * imageWidth + int(u * imageWidth)) * 3;
	if (u > 0.0 && v > 0.0)
		return { pixel[0]/255.0, pixel[1]/255.0, pixel[2]/255.0 };
	return {1.0, 1.0, 1.0};
}

Texture::Texture(const char* imageFileName, UV uv)
{
	imagePtr = readBmp(imageFileName, imageWidth, imageHeight);
}

vec3f Texture::getColorByUV(const double u, const double v)
{
	switch (uv)
	{
	case Box:
		return getBoxColor(u, v);
	case Square:
	default:
		return getSquareColor(u, v);
	}
}

vec3f Texture::getNormalByUV(const double u, const double v)
{
	auto fakeNormal = getColorByUV(u, v);
	fakeNormal[0] -= 0.5;
	fakeNormal[1] -= 0.5;
	fakeNormal[2] = 1.0 - (fakeNormal[2] - 0.5);
	return fakeNormal;
}
