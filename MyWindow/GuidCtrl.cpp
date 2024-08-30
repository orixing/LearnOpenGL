#include "GuidCtrl.h"
GuidCtrl& GuidCtrl::getInstance() {
	static GuidCtrl instance;
	return instance;
}

long GuidCtrl::GenGuid() {
	long ret = guid;
	guid += 1;
	return ret;
}

GuidCtrl::GuidCtrl(){}
GuidCtrl::~GuidCtrl(){}