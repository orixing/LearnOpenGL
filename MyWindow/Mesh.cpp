#include "Mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}


void Mesh::setupMesh()
{
    unsigned int amount = 1000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount]; 
    float radius = 10.0f;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model;
        // 1. λ�ƣ��ֲ��ڰ뾶Ϊ 'radius' ��Բ���ϣ�ƫ�Ƶķ�Χ�� [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // �����Ǵ��ĸ߶ȱ�x��z�Ŀ��ҪС
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. ���ţ��� 0.25 �� 0.75f ֮������
        float scale = (rand() % 50) / 100.0f + 0.25f;
        model = glm::scale(model, glm::vec3(scale));

        // 3. ��ת������һ�����룩���ѡ�����ת�����������������ת
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));

        // 4. ��ӵ������������
        modelMatrices[i] = model;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        &indices[0], GL_STATIC_DRAW);

    // ����λ��
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // ���㷨��
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // ������������
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));



    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), modelMatrices, GL_STATIC_DRAW);



        // ��������
        //GLsizei vec4Size = sizeof(glm::vec4);
        //glEnableVertexAttribArray(3);
        //glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        //glEnableVertexAttribArray(4);
        //glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
        //glEnableVertexAttribArray(5);
        //glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        //glEnableVertexAttribArray(6);
        //glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        //glVertexAttribDivisor(3, 1);
        //glVertexAttribDivisor(4, 1);
        //glVertexAttribDivisor(5, 1);
        //glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
}

void Mesh::Draw(Shader shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // �ڰ�֮ǰ������Ӧ������Ԫ
        // ��ȡ������ţ�diffuse_textureN �е� N��
        string number;
        string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);

        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // ��������
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, 1000);
    glBindVertexArray(0);
}