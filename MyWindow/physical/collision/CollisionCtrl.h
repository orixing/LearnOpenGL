#pragma once
#include "WindowContent.h"
class CollisionCtrl {
public:
	CollisionCtrl(const CollisionCtrl& c) = delete;
	CollisionCtrl& operator=(const CollisionCtrl & c) = delete;
	static CollisionCtrl& getInstance();

	WindowContent* content;

	void Tick(WindowContent* curContent);
private:
	~CollisionCtrl();
	CollisionCtrl();
};