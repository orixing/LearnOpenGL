#include "SkyboxMaterial.h"
SkyboxMaterial::SkyboxMaterial() : Material(MaterialEnum::Skybox) {

}

SkyboxMaterial::~SkyboxMaterial() {}

void SkyboxMaterial::SetTexture(TextureEnum e) {
	envHDRTex = TextureCtrl::getInstance().getTexture(e);
}
