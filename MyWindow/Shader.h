#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>; // ����glad����ȡ���еı���OpenGLͷ�ļ�

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>


class Shader
{
public:
    // ����ID
    unsigned int ID;

    // ��������ȡ��������ɫ��
    Shader(const char* vertexPath, const char* fragmentPath);
    // ʹ��/�������
    void use();
    // uniform���ߺ���
    void setBool(const std::string& name, const bool value) const;
    void setInt(const std::string& name, const int value) const;
    void setFloat(const std::string& name, const float value) const;
    void setMat4(const std::string& name, const glm::mat4 &value) const;
    void setVec3(const std::string& name, const glm::vec3 value) const;
    void setVec3(const std::string& name, const float x, const float y, const float z) const;
};

#endif