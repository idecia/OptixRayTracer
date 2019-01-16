#pragma once

#include "core/optix_global.h"
#include "lights/Light.h"
#include "shapes/TriangleMesh.h"
#include "shapes/Parallelogram.h"

/*
class EnvironmentLight : public Light {

public:

RT_FUNCTION EnvironmentLight();
RT_FUNCTION EnvironmentLight(const TriangleMesh &mesh, const float3 &color);
RT_FUNCTION float3 Sample(const float2 &sample);
RT_FUNCTION float3 Le(const float3& wo) const;
RT_FUNCTION float Area() const;
RT_FUNCTION float Pdf(const float3 &point) const;
RT_FUNCTION float3 Sample(const float3 &point, RNG &rng,
float3 &wo, float &pdf, float  &tMax);

private:

TriangleMesh mesh;
float3 color;


};

RT_FUNCTION EnvironmentLight::EnvironmentLight() { }

RT_FUNCTION EnvironmentLight::EnvironmentLight(const TriangleMesh &mesh, const float3 &color)
:Light(Light::AREA), mesh(mesh), color(color) {

}

RT_FUNCTION float EnvironmentLight::Area() const {

return mesh.Area();

}

RT_FUNCTION float3 EnvironmentLight::Le(const float3& wo) const {

return color;
}

RT_FUNCTION float EnvironmentLight::Pdf(const float3 &point) const {

return 1.0f / Area();

}

RT_FUNCTION float3 EnvironmentLight::Sample(const float3 &point, RNG &rng,
float3 &wo, float &pdf, float  &tMax)  {

float3 meshPoint, normal;
mesh.Sample(rng, meshPoint, normal);
wo = meshPoint - point;
tMax  = length(wo);
float distanceSquared = tMax * tMax;
wo = normalize(wo);
pdf = distanceSquared / (dot(normal, -wo) * Area());
float3 Le = dot(normal, -wo) > 0 ? color : make_float3(0.0f);
return Le;

}
*/

class EnvironmentLight : public Light {

public:

	RT_FUNCTION EnvironmentLight();
	RT_FUNCTION EnvironmentLight(const Parallelogram &parallelogram);
	RT_FUNCTION float Pdf(const float3 &point) const;
	RT_FUNCTION float3 NormalAt(const float3 &point) const;
	RT_FUNCTION float3 Sample(const float3 &point, const float2 &sample,
		float3 &wo, float &pdf, float  &tMax);


//private:

	Parallelogram parallelogram;


};

RT_FUNCTION EnvironmentLight::EnvironmentLight() { }

RT_FUNCTION EnvironmentLight::EnvironmentLight(const Parallelogram &parallelogram)
	:parallelogram(parallelogram) {

}

RT_FUNCTION float3 EnvironmentLight::Sample(const float3 &point, const float2 &sample,
	float3 &wo, float &pdf, float  &tMax)  {

	float3 ps = parallelogram.Sample(sample);
	float3 n = parallelogram.NormalAt(ps);
	wo = ps - point;
	tMax = length(wo);
	float distanceSquared = tMax * tMax;
	wo = normalize(wo);
	pdf = distanceSquared / (dot(n, -wo) * parallelogram.GetArea());
	float3 L = dot(n, -wo) > 0 ? make_float3(1.0f) : make_float3(0.0f);
	return L;

}

RT_FUNCTION float3 EnvironmentLight::NormalAt(const float3 &point) const {

	return parallelogram.NormalAt(point);

}



RT_FUNCTION float EnvironmentLight::Pdf(const float3 &point) const {

	float area = parallelogram.GetArea();
	return 1.0f / area;

}