#ifndef __PERLIN_INCLUDED__
#define __PERLIN_INCLUDED__

namespace Perlin
{
	void FillGradientVectors(float*, int, int);
	void GeneratePerlin(float*, int, int, float, double);
}

#endif