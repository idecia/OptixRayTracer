#pragma once

#include "core/optix_global.h"
#include "core/Ray.h"

using namespace optix;

struct BSFDQueryRecord {
	float3 point;
	float3 wi;
	float3 wo;
};

class Lambertian {

public:

	RT_FUNCTION Lambertian();
	RT_FUNCTION Lambertian(const float3 &color);
	RT_FUNCTION float Pdf(const BSFDQueryRecord &record) const;
	RT_FUNCTION float3 Sample(BSFDQueryRecord &record, const float2 &sample) const;
	RT_FUNCTION float3 Eval(const BSFDQueryRecord &record) const;

private:

	float3 color;

};

RT_FUNCTION Lambertian::Lambertian() { }

RT_FUNCTION Lambertian::Lambertian(const float3 &color)
	:color(color) {

}

RT_FUNCTION float Lambertian::Pdf(const BSFDQueryRecord &record) const {
	
	return  M_1_PIf;
}

RT_FUNCTION float3 Lambertian::Sample(BSFDQueryRecord &record, const float2 &sample) const {

	//TODO: sample hemisphere using sample
	//return M_1_PIf * color;  <-- This is the correct term
	return color;
}

RT_FUNCTION float3 Lambertian::Eval(const BSFDQueryRecord &record) const {

	//return M_1_PIf * color;  <-- This is the correct term
	return color;
}

