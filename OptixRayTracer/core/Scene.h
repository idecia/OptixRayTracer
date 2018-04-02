#pragma once
#include "core/optix_global.h"

class Scene {

public:
	RT_FUNCTION Scene();
	RT_FUNCTION Scene(Context context);
	RT_FUNCTION void Render();
	RT_FUNCTION Buffer GetOutputImage();
	RT_FUNCTION void SetWidth(int width);
	RT_FUNCTION void SetHeight(int height);
	RT_FUNCTION int GetWidth();
	RT_FUNCTION int GetHeight();


private:
	Context context;
	int width;
	int height;
};

RT_FUNCTION  Scene::Scene() {

}

RT_FUNCTION  Scene::Scene(Context context) 
	: context(context) {

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
	context->launch(0, width, height);

}

RT_FUNCTION Buffer Scene::GetOutputImage() {

	return context["output"]->getBuffer();
}