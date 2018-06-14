#pragma once

#include "core/optix_global.h"
#include "core/BxDF.h"
#include "samplers/Util.h"


using namespace optix;


class Lambertian {

public:

   RT_FUNCTION Lambertian();
   RT_FUNCTION Lambertian(const float3 &color);
   RT_FUNCTION float Pdf(const float3 &wo, const float3 &wi) const;
   RT_FUNCTION float3 Sample(const float3 &wo, float3 *wi, float *pdf, const float2 &sample) const;
   RT_FUNCTION float3 Eval(const float3 &wo, const float3 &wi) const;
   RT_FUNCTION float3 getR();

private:

	float3 R;
	float3 RoverPI;

};

RT_FUNCTION float3 Lambertian::getR() {
	return R;
}


RT_FUNCTION Lambertian::Lambertian() { }

RT_FUNCTION Lambertian::Lambertian(const float3 &reflectance)
	:R(reflectance), RoverPI(reflectance * M_1_PIf) {

}

RT_FUNCTION float Lambertian::Pdf(const float3 &wo, const float3 &wi) const {
   
	return fabsf(wi.z) * M_1_PIf;
}

RT_FUNCTION float3 Lambertian::Sample(const float3 &wo, float3 *wi, float *pdf, const float2 &sample) const {

	*wi  = CosineHemisphereSample(sample.x, sample.y);
	*pdf = Pdf(wo, *wi);
	return Eval(wo, *wi);
}

RT_FUNCTION float3 Lambertian::Eval(const float3 &wo, const float3 &wi) const {

   return RoverPI; 
}


/*

class Lambertian : public BxDF {

public:


	RT_FUNCTION Lambertian();
	RT_FUNCTION Lambertian(const float3 &reflectance);
	RT_FUNCTION float3 f(const float3 &wo, const float3 &wi) const;
	RT_FUNCTION float3 Sample(const float3 &wo, float3 *wi, float u1, float u2, float *pdf) const;
	RT_FUNCTION float Pdf(const float3 &wo, const float3 &wi) const;
	RT_FUNCTION float3 Rho(const float3 &wo, int nSamples = 16, float *samples = NULL);
	RT_FUNCTION float3 Rho(int nSamples = 16, float *samples = NULL);

private:

	float3 R;
	float3 RoverPI;

};


RT_FUNCTION  Lambertian::Lambertian() {

}

RT_FUNCTION Lambertian::Lambertian(const float3 &reflectance)
	: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
	  R(reflectance), RoverPI(reflectance * M_1_PIf) {

}

RT_FUNCTION float3 Lambertian::f(const float3 &wo, const float3 &wi) const {

	return RoverPI;
}

RT_FUNCTION float3 Lambertian::Sample(const float3 &wo, float3 *wi, float u1, float u2, float *pdf) const {

	*wi  = UniformHemisphereSample(u1, u2);
	*pdf = Pdf(wo, *wi);
	return f(wo, *wi);

}


RT_FUNCTION float Lambertian::Pdf(const float3 &wo, const float3 &wi)  const {

	return fabsf(wi.z) * M_1_PIf;

}


RT_FUNCTION float3 Lambertian::Rho(const float3 &wo, int nSamples, float *samples) {

	return R;
}

RT_FUNCTION float3 Lambertian::Rho(int nSamples, float *samples) {

	return R;
}
*/