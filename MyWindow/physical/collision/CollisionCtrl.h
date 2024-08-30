#pragma once
#include "WindowContent.h"
#include <unordered_set>;
class CollisionCtrl {
public:
	CollisionCtrl(const CollisionCtrl& c) = delete;
	CollisionCtrl& operator=(const CollisionCtrl & c) = delete;
	static CollisionCtrl& getInstance();

	WindowContent* content = nullptr;

	std::unordered_set<GameObj*> collisionObjs;//所有参与碰撞的物体
	std::vector<std::pair<GameObj*, GameObj*>> collisionPairCache;//当前帧所有已经检测过的物体对
	std::vector<CollisionEvent*> collisionEvents;//当前帧所有碰撞事件
	void Tick(WindowContent* curContent);
private:
	~CollisionCtrl();
	CollisionCtrl();
};