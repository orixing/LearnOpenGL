#include "Material.h"

Material::Material(MaterialEnum e):materialEnum(e){};

Material::~Material() {}

void Material::DrawInGPass(Shader* shader){}

void Material::Draw() {}
