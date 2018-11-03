#pragma once

#include "core/optix_global.h"
#include "core/ONB.h"
#include "core/BxDF.h"

#define MAX_BxDFs = 8

class BSDF {

public:
	
	RT_FUNCTION BSDF();
	RT_FUNCTION BSDF(float3 normal);
	RT_FUNCTION void Add(const BxDF & bxdf) const;
	RT_FUNCTION bool MatchesFlags(const BxDF & bxdf, BxDFType flags) const;
	RT_FUNCTION int NumComponents() const;
	RT_FUNCTION int NumComponents(BxDFType flags) const;

private:

	ONB onb;
	BxDF bxdfs[MAX_BxDFs];
	int nBxDFs;

};


