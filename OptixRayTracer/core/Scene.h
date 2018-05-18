#pragma once
#include "core/optix_global.h"

class Scene {

public:
	RT_FUNCTION Scene();
	RT_FUNCTION Scene(Context context, Buffer coeff);
	RT_FUNCTION void Render();
	RT_FUNCTION Buffer GetOutputImage();
	RT_FUNCTION Buffer GetSensorValues();
	RT_FUNCTION void SetWidth(int width);
	RT_FUNCTION void SetHeight(int height);
	RT_FUNCTION int GetWidth();
	RT_FUNCTION int GetHeight();
	RT_FUNCTION void ChangeSensorPosition(float3 position);
	RT_FUNCTION void ResetSensorValues();

private:
	Context context;
	int width;
	int height;

	Buffer coeff;
	
};

RT_FUNCTION  Scene::Scene() {

}

RT_FUNCTION  Scene::Scene(Context context, Buffer coeff) 
	: context(context), coeff(coeff) {

}

RT_FUNCTION int Scene::GetWidth() {
	return width;
}

RT_FUNCTION int Scene::GetHeight() {
	return height;
}


RT_FUNCTION void Scene::SetWidth(int width) {
	this->width = width;
}

RT_FUNCTION void Scene::SetHeight(int height) {
	this->height = height;
}


RT_FUNCTION void Scene::Render() {

	context->setPrintEnabled(true);
	context->validate();
	context->compile();
	if (height == 0)
		context->launch(0, width);
	else
		context->launch(0, width, height);

}



RT_FUNCTION Buffer Scene::GetOutputImage() {

	return context["output"]->getBuffer();
}

RT_FUNCTION Buffer Scene::GetSensorValues() {

	return context["coeff"]->getBuffer();
}

RT_FUNCTION void Scene::ChangeSensorPosition(float3 position) {

	context["sensorPos"]->setFloat(position);

}

RT_FUNCTION void Scene::ResetSensorValues() {

	int NskyPatches = 146;
	float* values = (float*)coeff->map();
	for (unsigned int i = 0; i < NskyPatches * 3; i++) {
		values[i] = 0.0f;
	}
	coeff->unmap();
}
