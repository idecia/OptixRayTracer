#pragma once

#include "core/optix_global.h"
#include "core/BxDF.h"
#include "FresnelDielectric.h"

using namespace optix;


class ThinGlass {

public:

	RT_FUNCTION ThinGlass();
	RT_FUNCTION ThinGlass(float R0, float T0, float d, float lambda);
	RT_FUNCTION float Pdf(const float3 &wo, const float3 &wi) const;
	RT_FUNCTION float3 Sample(const float3 &wo, float3 *wi, float *pdf, const float2 &sample) const;
	RT_FUNCTION float3 Eval(const float3 &wo, const float3 &wi) const;

	FresnelDielectric F;
private:
	//FresnelDielectric F;
};





RT_FUNCTION ThinGlass::ThinGlass() {

}

RT_FUNCTION ThinGlass::ThinGlass(float R0, float T0, float d, float lambda) {

	float n, k;
	FresnelDielectric::EstimateRefractionIndex(R0, T0, lambda, d, &n, &k);
	F = FresnelDielectric(n, k, lambda, d);

}


RT_FUNCTION float ThinGlass::Pdf(const float3 &wo, const float3 &wi) const {

	return 0.0f;

}

RT_FUNCTION float3 ThinGlass::Eval(const float3 &wo, const float3 &wi) const {

	return make_float3(0.0f);

}

RT_FUNCTION float3 ThinGlass::Sample(const float3 &wo, float3 *wi, float *pdf, const float2 &sample) const {

	*wi = -wo;
	*pdf = 1.0f;
	float T = F.Transmittance(AbsCosTheta(wo));
	return make_float3(1 / AbsCosTheta(*wi));
	
}