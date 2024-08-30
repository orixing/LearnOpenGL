#include "CollisionCtrl.h"

CollisionCtrl& CollisionCtrl::getInstance() {
	static CollisionCtrl instance;
	return instance;
}

CollisionCtrl::~CollisionCtrl(){}
CollisionCtrl::CollisionCtrl(){}

void CollisionCtrl::Tick(WindowContent* curContent) {
	content = curContent;

    //所有动态物体计算包围盒
    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
        obj->physical->BuildBoundingBox();
    }

    //动态物体做碰撞前的处理，由静变动的物体由OnDynamic()初始化
    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
        obj->physical->BeforeCollision();
    }

    collisionObjs.clear();
    collisionPairCache.clear();
    collisionEvents.clear();

    //检查运动物体和其他物体的碰撞，存储碰撞结果
    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
        for (GameObj* other : *content->allObjs) {
            if (other->physical == NULL) continue;
            if (obj == other) continue;
            //已经检测过A与B，不再检测B与A
            bool alreadyDetect = false;
            for (const std::pair<GameObj*, GameObj*>& pair : collisionPairCache) {
                if (pair.first == other && pair.second == obj) {
                    alreadyDetect = true;
                    break;
                }
            }
            if(alreadyDetect) continue;
            //开始碰撞检测，存储检测记录，避免重复检测
            collisionPairCache.push_back(std::pair<GameObj*, GameObj*>(obj, other));
            //先检测包围盒，如果不重合就检测别的物体
            if (!obj->physical->boundingBox->CollisionDetect(other->physical->boundingBox))
                continue;
            //包围盒重合，详细检测
            CollisionEvent* event = nullptr;

            //两个撞击物体根据自身物理组件共同决定DCD碰撞检测策略，使用双分派实现
            if (!obj->physical->DiscreteCollisionDetect(other->physical, &event))
                continue;//包围盒重合但实际未碰撞
            //实际产生碰撞，存储碰撞事件
            collisionEvents.push_back(event);
            collisionObjs.insert(obj);
            collisionObjs.insert(other);
        }
    }

    //处理所有碰撞
    for (CollisionEvent* event : collisionEvents) {
        event->obj1->HandleCollsion(event->obj2, event);
    }

    //碰撞后的移动
    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
        obj->physical->AfterCollision();
    }

    //for (GameObj* obj : *content->allObjs) {
    //    if (obj->physical == NULL) continue;
    //    if (!obj->physical->IsDynamic()) continue;
    //    obj->physical->BeforeCollision();

    //    //这里先只做地面和物体之间的碰撞检测
    //    for (GameObj* other : *content->allObjs) {
    //        if (other->physical == NULL) continue;
    //        if (obj == other) continue;
    //        if (obj->physical->boundingBox->CollisionDetect(other->physical->boundingBox))
    //            obj->physical->HandleCollision(other);
    //    }

    //    obj->physical->AfterCollision();
    //}
}