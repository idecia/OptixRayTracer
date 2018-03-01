#pragma once

#include "core/optix_global.h"

class Sphere {

public:

	RT_FUNCTION Sphere();
	RT_FUNCTION Sphere(const float3 &center, float radius);
	RT_FUNCTION float3 Sphere::NormalAt(const float3 &point);
	RT_FUNCTION float3 GetCenter() const;
	RT_FUNCTION float GetRadius() const;
	

private:
	float3 center;
	float radius;

};


RT_FUNCTION Sphere::Sphere() { }

RT_FUNCTION Sphere::Sphere(const float3 &center, float radius) 
	: center(center), radius(radius) {

}

RT_FUNCTION float3 Sphere::NormalAt(const float3 &point) {
	return (point - center) / radius;
}

RT_FUNCTION float3 Sphere::GetCenter() const {
	return center;
}

RT_FUNCTION float Sphere::GetRadius() const {
	return radius;
}