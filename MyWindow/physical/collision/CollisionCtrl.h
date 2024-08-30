#pragma once
#include "WindowContent.h"
#include <unordered_set>;
class CollisionCtrl {
public:
	CollisionCtrl(const CollisionCtrl& c) = delete;
	CollisionCtrl& operator=(const CollisionCtrl & c) = delete;
	static CollisionCtrl& getInstance();

	WindowContent* content = nullptr;

	std::unordered_set<GameObj*> collisionObjs;//���в�����ײ������
	std::vector<std::pair<GameObj*, GameObj*>> collisionPairCache;//��ǰ֡�����Ѿ������������
	std::vector<CollisionEvent*> collisionEvents;//��ǰ֡������ײ�¼�
	void Tick(WindowContent* curContent);
private:
	~CollisionCtrl();
	CollisionCtrl();
};