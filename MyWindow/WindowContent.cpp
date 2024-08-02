#include "WindowContent.h"


WindowContent::WindowContent(Camera* c) :mainCamera(c) {
	allObjs = new std::vector<GameObj*>();
}
WindowContent::~WindowContent() {}