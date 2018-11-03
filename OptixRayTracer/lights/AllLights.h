#pragma once

#include "lights/Light.h"
#include "lights/AreaLight.h"
#include "lights/PointLight.h"
#include <algorithm>

unsigned int LightSizes[] = {
   sizeof(AreaLight),
   sizeof(PointLight)
};

const unsigned int MAX_LIGHT_SIZE = 
   *std::max_element(LightSizes, LightSizes + 2);