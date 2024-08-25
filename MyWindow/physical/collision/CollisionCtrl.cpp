#include "CollisionCtrl.h"

CollisionCtrl& CollisionCtrl::getInstance() {
	static CollisionCtrl instance;
	return instance;
}

CollisionCtrl::~CollisionCtrl(){}
CollisionCtrl::CollisionCtrl(){}

void CollisionCtrl::Tick(WindowContent* curContent) {
	content = curContent;

    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        //if (!obj->physical->IsDynamic()) continue;
        obj->physical->BuildBoundingBox();
    }

    //检查运动物体和其他物体的碰撞，存储碰撞结果
    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
    }

    //处理所有碰撞


    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
        obj->physical->BeforeCollision();

        //这里先只做地面和物体之间的碰撞检测
        for (GameObj* other : *content->allObjs) {
            if (other->physical == NULL) continue;
            if (obj == other) continue;
            if (obj->boundingBox->CollisionDetect(other->boundingBox))
                obj->physical->HandleCollision(other);
        }

        obj->physical->AfterCollision();
    }
}