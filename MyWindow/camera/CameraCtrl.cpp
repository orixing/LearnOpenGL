#include "CameraCtrl.h"
CameraCtrl& CameraCtrl::getInstance() {
	static CameraCtrl instance;
	return instance;
}

CameraCtrl::CameraCtrl() {
	
}
CameraCtrl::~CameraCtrl() {}
