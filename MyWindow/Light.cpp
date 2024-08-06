#include "Light.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

glm::mat4 Light::GetLightSpaceMatrix() {
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
    return lightProjection * lightView;
}

glm::mat4 Light::GetLightProjection() {
    return glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
}


Light::Light(glm::vec3 lightPos) : lightPos(lightPos){
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    Texture* depthMapTex = Texture::Builder().SetInternalFormat(GL_DEPTH_COMPONENT).SetFormat(GL_DEPTH_COMPONENT).SetWidth(ShadowMapWidth)
        .SetHeight(ShadowMapHeight).SetName("depthMap").SetWrapS(GL_CLAMP_TO_BORDER).SetWrapT(GL_CLAMP_TO_BORDER).SetBorder(borderColor).Build();
    depthMapFBO = FrameBuffer::Builder().BindDepthTexture(depthMapTex).NeedColorTexture(false).Build();
}
Light::~Light() {}