#include "WindowContent.h"


WindowContent::WindowContent(Camera* c) :mainCamera(c) {
	allObjs = new std::vector<GameObj*>();
	allLights = new std::vector<Light*>();
}
WindowContent::~WindowContent() {}