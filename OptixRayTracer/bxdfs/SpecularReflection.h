#pragma once

#include "core/optix_global.h"
#include "core/BxDF.h"

class SpecularReflection : public BxDF {

public:

	RT_FUNCTION SpecularReflection(const float3 &reflectivity);
	RT_FUNCTION float3 f(const float3 &wo, const float3 &wi) const;
	RT_FUNCTION float3 Sample(const float3 &wo, float3 *wi, float u1, float2 u2, float *pdf) const;
	RT_FUNCTION float Pdf(const float3 &wo, const float3 &wi);
	RT_FUNCTION float3 Rho(const float3 &wo, int nSamples = 16, float *samples = NULL);
	RT_FUNCTION float3 Rho(int nSamples = 16, float *samples = NULL);
	RT_FUNCTION float3 Reflect(const float3 &v);

private:

	float3 R;

}

RT_FUNCTION SpecularReflection::SpecularReflection(const float3 &reflectivity)
	: BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), 
	  R(reflectivity) {
}

RT_FUNCTION float3 SpecularReflection::f(const float3 &wo, const float3 &wi) const {

	return 0.0f;
}


RT_FUNCTION float3 SpecularReflection::Sample(const float3 &wo, float3 *wi, 
	float u1, float2 u2, float *pdf) const {

	*wi = Reflect(wo);
	*pdf = 1.0f;
	return R / fabsf(CosTheta(wo));

}

RT_FUNCTION float Pdf(const float3 &wo, const float3 &wi) {

	return 0.0f;

}

RT_FUNCTION float3 SpecularReflection::Rho(const float3 &wo, int nSamples = 16, 
	float *samples = NULL) {

	return R;
}

RT_FUNCTION float3 SpecularReflection::Rho(int nSamples = 16, float *samples = NULL) {
	
	return R;

}

RT_FUNCTION float3 SpecularReflection::Reflect(const float3 &v) {

	return make_float3(-v.x, -v.y, v.z);
}