#pragma once
#include <string>
class Texture
{
public:
	unsigned int id;
	std::string name;
	~Texture();
	Texture(unsigned int id, const std::string& name);
};