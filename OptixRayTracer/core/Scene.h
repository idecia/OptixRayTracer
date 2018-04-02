#pragma once
#include "core/optix_global.h"

class Scene {

public:
	RT_FUNCTION Scene();
	RT_FUNCTION Scene(Context context);

private:
	Context context;
};

RT_FUNCTION  Scene::Scene() {

}

RT_FUNCTION  Scene::Scene(Context context) 
	: context(context) {

}