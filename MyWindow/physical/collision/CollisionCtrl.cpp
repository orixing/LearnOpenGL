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

    //����˶�����������������ײ���洢��ײ���
    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
    }

    //����������ײ


    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
        obj->physical->BeforeCollision();

        //������ֻ�����������֮�����ײ���
        for (GameObj* other : *content->allObjs) {
            if (other->physical == NULL) continue;
            if (obj == other) continue;
            if (obj->boundingBox->CollisionDetect(other->boundingBox))
                obj->physical->HandleCollision(other);
        }

        obj->physical->AfterCollision();
    }
}