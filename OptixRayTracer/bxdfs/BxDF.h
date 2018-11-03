#pragma once

#include "core/optix_global.h"

/*
class BxDF {

public:
	enum BxDFType {
		
		BSDF_REFLECTION = 1 << 0,
		BSDF_TRANSMISSION = 1 << 1,
		BSDF_DIFFUSE = 1 << 2,
		BSDF_GLOSSY = 1 << 3,
		BSDF_SPECULAR = 1 << 4,
		BSDF_ALL_TYPES = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR,
		BSDF_ALL_REFLECTION = BSDF_REFLECTION | BSDF_ALL_TYPES,
		BSDF_ALL_TRANSMISSION = BSDF_TRANSMISSION | BSDF_ALL_TYPES,
		BSDF_ALL = BSDF_ALL_REFLECTION | BSDF_ALL_TRANSMISSION
	}; 

	RT_FUNCTION BxDF();
	RT_FUNCTION BxDF(BxDFType t);
	RT_FUNCTION BxDFType GetType() const;
	RT_FUNCTION bool MatchesFlags(BxDFType flags) const;
	RT_FUNCTION float CosTheta(const float3 &w) const;

private:

	BxDFType type;


};


RT_FUNCTION BxDF::BxDF() { }

RT_FUNCTION BxDF::BxDF(BxDFType type) : type(type) { }

RT_FUNCTION bool BxDF::MatchesFlags(BxDFType flags) const {

	return (type & flags) == type;
}

RT_FUNCTION float BxDF::CosTheta(const float3& w) const {

	return w.z;
}

RT_FUNCTION BxDFType BxDF::GetType() const {

	return type;

}

*/

RT_FUNCTION float CosTheta(const float3& w)  {

	return w.z;
}

RT_FUNCTION float AbsCosTheta(const float3& w)  {

	return fabsf(w.z);
}