#pragma once

#include "core/optix_global.h"

using namespace optix;


class FresnelDielectric {

public:
	RT_FUNCTION FresnelDielectric();
	RT_FUNCTION FresnelDielectric(float n, float k, float lambda, float d);
	RT_FUNCTION float Transmittance(float coso) const;
	RT_FUNCTION float Reflectance(float coso) const;
	RT_FUNCTION float Reflectivity(float coso, float cost) const;
	RT_FUNCTION float Transmissivity(float coso) const;
	RT_FUNCTION float Transmittance(float coso, float cost) const;
	RT_FUNCTION float Reflectivity(float coso) const;
	RT_FUNCTION float SnellCosine(float coso) const;

	static RT_FUNCTION float EstimateNormalReflectivity(float R0, float T0);
	static RT_FUNCTION void EstimateRefractionIndex(float R0, float T0, float lambda, float d, 
		float* n, float* k); 

private:
	float n;
	float k;
	float lambda;
	float d;
	float alpha;
	float pad;
	
};

RT_FUNCTION FresnelDielectric::FresnelDielectric() {

}

RT_FUNCTION FresnelDielectric::FresnelDielectric(float n, float k, float lambda, float d) 
	:n(n), k(k), lambda(lambda), d(d), alpha(4*M_PIf*k/lambda) {

}

RT_FUNCTION float FresnelDielectric::Reflectivity(float coso, float cost) const {

	float Rperp = (n*cost - coso) / (n*cost + coso);
	float Rparl = (n*coso - cost) / (n*coso + cost);
	return (Rperp*Rperp + Rparl*Rparl) * 0.5f;

}

RT_FUNCTION float FresnelDielectric::Transmittance(float coso, float cost) const {

	float r = Reflectivity(coso, cost);
	float t = 1.0f - r;
	return t*t*exp(-alpha*d / cost) / (1.0f - r*r*exp(-2.0f * alpha*d / cost));

}

RT_FUNCTION float FresnelDielectric::Reflectivity(float coso) const {

	float cost = SnellCosine(coso);
	return Reflectivity(coso, cost);

}

RT_FUNCTION float FresnelDielectric::Transmissivity(float coso)  const{

	return 1.0f - Reflectivity(coso);

}

RT_FUNCTION float FresnelDielectric::Reflectance(float coso) const {

	float cost = SnellCosine(coso);
	float r = Reflectivity(coso, cost);
	float T = Transmittance(coso, cost);
	return r*(1.0f + exp(-alpha*d / cost)*T);

}


RT_FUNCTION float FresnelDielectric::Transmittance(float coso) const {

	float cost = SnellCosine(coso);
	return Transmittance(coso, cost);

}

RT_FUNCTION float FresnelDielectric::EstimateNormalReflectivity(float R0, float T0)  {

	float beta = T0*T0 - R0*R0 + 2*R0 + 1;
	return (beta - sqrtf(beta*beta - 4*(2 - R0)*R0)) / (2*(2 - R0));
}

RT_FUNCTION void FresnelDielectric::EstimateRefractionIndex(float R0, float T0, float lambda, 
	float d, float* n, float* k)  {

	float r0 = EstimateNormalReflectivity(R0, T0);
	*n = (1 + sqrtf(r0)) / (1 - sqrtf(r0));
	*k = -(lambda / (4*M_PIf*d)) * logf((R0 - r0) / (r0*T0));
}

RT_FUNCTION float FresnelDielectric::SnellCosine(float coso) const {

	float sint = (1.0f / n) * sqrt(max(0.0f, 1.0f - coso*coso));
	return sqrtf(max(0.f, 1.0f - sint*sint));
}
