#ifndef __FRACTAL_FAULT_INCLUDED__
#define __FRACTAL_FAULT_INCLUDED__

#include <d3d11.h>

namespace Generation
{
	class FractalFault
	{
	private:
		//Fault information
		struct Fault
		{
			float offset;
			float grad;
			bool gt;
		};

		ID3D11ComputeShader* m_shader;
		ID3D11UnorderedAccessView* m_faults;
		ID3D11UnorderedAccessView* m_RWTex;

	public:
		static FractalFault* instance;

		FractalFault();
		~FractalFault();

		FractalFault(const FractalFault&) = delete;
		FractalFault(const FractalFault&&) = delete;
		FractalFault& operator=(const FractalFault&) = delete;
		FractalFault& operator=(const FractalFault&&) = delete;

		static void Generate(float*, int, int, int);
		static void GenerateGPU(float*, int, int, int);
	};
}

#endif
