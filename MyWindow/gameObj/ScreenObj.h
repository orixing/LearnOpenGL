#pragma once
#include "GameObj.h"
class ScreenObj : public GameObj
{
public:
	ScreenObj(const ScreenObj& obj) = delete;
	ScreenObj& operator=(const ScreenObj&) = delete;

	static ScreenObj& getInstance();

	void Draw() override;

private:
	ScreenObj();
	~ScreenObj();
	unsigned int screenVAO;
};

