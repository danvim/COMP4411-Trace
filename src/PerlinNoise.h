#pragma once

// From https://mrl.nyu.edu/~perlin/noise/

class PerlinNoise final
{
public:
	static double noise(double x, double y, double z);
	static double fade(double t);
	static double lerp(double t, double a, double b);
	static double grad(int hash, double x, double y, double z);
	static int* p;
	static const int PERMUTATION[];
};
