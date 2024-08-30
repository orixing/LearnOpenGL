#pragma once
class GuidCtrl
{
public:
	GuidCtrl(const GuidCtrl& c) = delete;
	GuidCtrl& operator=(const GuidCtrl& c) = delete;

	static GuidCtrl& getInstance();
	long GenGuid();
private:
	long guid = 0;
	GuidCtrl();
	~GuidCtrl();
};

