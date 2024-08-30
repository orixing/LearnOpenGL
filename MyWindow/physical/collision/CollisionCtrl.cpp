#include "CollisionCtrl.h"

CollisionCtrl& CollisionCtrl::getInstance() {
	static CollisionCtrl instance;
	return instance;
}

CollisionCtrl::~CollisionCtrl(){}
CollisionCtrl::CollisionCtrl(){}

void CollisionCtrl::Tick(WindowContent* curContent) {
	content = curContent;

    //���ж�̬��������Χ��
    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
        obj->physical->BuildBoundingBox();
    }

    //��̬��������ײǰ�Ĵ����ɾ��䶯��������OnDynamic()��ʼ��
    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
        obj->physical->BeforeCollision();
    }

    collisionObjs.clear();
    collisionPairCache.clear();
    collisionEvents.clear();

    //����˶�����������������ײ���洢��ײ���
    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
        for (GameObj* other : *content->allObjs) {
            if (other->physical == NULL) continue;
            if (obj == other) continue;
            //�Ѿ�����A��B�����ټ��B��A
            bool alreadyDetect = false;
            for (const std::pair<GameObj*, GameObj*>& pair : collisionPairCache) {
                if (pair.first == other && pair.second == obj) {
                    alreadyDetect = true;
                    break;
                }
            }
            if(alreadyDetect) continue;
            //��ʼ��ײ��⣬�洢����¼�������ظ����
            collisionPairCache.push_back(std::pair<GameObj*, GameObj*>(obj, other));
            //�ȼ���Χ�У�������غϾͼ��������
            if (!obj->physical->boundingBox->CollisionDetect(other->physical->boundingBox))
                continue;
            //��Χ���غϣ���ϸ���
            CollisionEvent* event = nullptr;

            //����ײ����������������������ͬ����DCD��ײ�����ԣ�ʹ��˫����ʵ��
            if (!obj->physical->DiscreteCollisionDetect(other->physical, &event))
                continue;//��Χ���غϵ�ʵ��δ��ײ
            //ʵ�ʲ�����ײ���洢��ײ�¼�
            collisionEvents.push_back(event);
            collisionObjs.insert(obj);
            collisionObjs.insert(other);
        }
    }

    //����������ײ
    for (CollisionEvent* event : collisionEvents) {
        event->obj1->HandleCollsion(event->obj2, event);
    }

    //��ײ����ƶ�
    for (GameObj* obj : *content->allObjs) {
        if (obj->physical == NULL) continue;
        if (!obj->physical->IsDynamic()) continue;
        obj->physical->AfterCollision();
    }

    //for (GameObj* obj : *content->allObjs) {
    //    if (obj->physical == NULL) continue;
    //    if (!obj->physical->IsDynamic()) continue;
    //    obj->physical->BeforeCollision();

    //    //������ֻ�����������֮�����ײ���
    //    for (GameObj* other : *content->allObjs) {
    //        if (other->physical == NULL) continue;
    //        if (obj == other) continue;
    //        if (obj->physical->boundingBox->CollisionDetect(other->physical->boundingBox))
    //            obj->physical->HandleCollision(other);
    //    }

    //    obj->physical->AfterCollision();
    //}
}