#pragma once
class GlobalConst
{
public:
	static const int ScreenWidth = 800; 
	static const int ScreenHeight = 600;
	static const int CubeMapCacheSize = 1024;
	static const int diffuseCubeSize = 128;
	static const int specularCubeSize = 128;
	static const int specularBRDFTexSize = 512;
private:
	GlobalConst() = delete;
	~GlobalConst() = delete;
};

