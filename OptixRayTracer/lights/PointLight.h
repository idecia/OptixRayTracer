#pragma once

#include "core/optix_global.h"

class PointLight {

public:

	RT_FUNCTION PointLight();
	RT_FUNCTION PointLight(const float3 &position, float3 color); 
	RT_FUNCTION float3 GetPosition() const;
	RT_FUNCTION float3 GetColor() const;

	
private:

	float3 position;
	float3 color;
	

};

RT_FUNCTION PointLight::PointLight() { }

RT_FUNCTION PointLight::PointLight(const float3 &position, float3 color)
   :position(position), color(color) {

}

RT_FUNCTION float3 PointLight::GetPosition() const {

	return position;
}

RT_FUNCTION float3 PointLight::GetColor() const {

	return color;
}

