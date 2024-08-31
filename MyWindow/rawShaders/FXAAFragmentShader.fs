#version 330 core

// ����İ�����Ⱦ����������
uniform sampler2D screenTexture;
in vec2 TexCoords;
out vec4 FragColor;


const float FXAA_THRESHOLD = 1.0/8.0;
const float FXAA_EDGE_THRESHOLD_MIN = 1.0/16.0;

void main() {
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);

    float FXAA_SPAN_MAX = 8.0;
    float FXAA_REDUCE_MIN = 1.0 / 128.0;
    float FXAA_REDUCE_MUL = 1.0 / 8.0;

    vec3 rgbM = texture(screenTexture, TexCoords).xyz;
    float lumaM = dot(rgbM, vec3(0.299, 0.587, 0.114));

    vec3 rgbN = texture(screenTexture, TexCoords + (vec2(0.0, -1.0) * texelSize)).xyz;
    vec3 rgbW = texture(screenTexture, TexCoords + (vec2(-1.0, 0.0) * texelSize)).xyz;
    vec3 rgbS = texture(screenTexture, TexCoords + (vec2(0.0,  1.0) * texelSize)).xyz;
    vec3 rgbE = texture(screenTexture, TexCoords + (vec2(1.0,  0.0) * texelSize)).xyz;
    float lumaN = dot(rgbN, vec3(0.299, 0.587, 0.114));
    float lumaW = dot(rgbW, vec3(0.299, 0.587, 0.114));
    float lumaS = dot(rgbS, vec3(0.299, 0.587, 0.114));
    float lumaE = dot(rgbE, vec3(0.299, 0.587, 0.114));

    //������Ⱥ���С����
    float lumaMin = min(lumaM, min(min(lumaN, lumaE), min(lumaS, lumaW)));
    float lumaMax = max(lumaM, max(max(lumaN, lumaE), max(lumaS, lumaW)));
    float localContrast = lumaMax - lumaMin;

    //��С����ֵ����������
    if(localContrast < max(FXAA_THRESHOLD, FXAA_EDGE_THRESHOLD_MIN * lumaMax)){
        FragColor = vec4(pow(rgbM, vec3(1.0/2.2)),1.0);
        return;
    }

    //float pixelContrast = abs(lumaM - 0.25*(lumaN+lumaW+lumaS+lumaE));
    //pixel contrast��local contrast�ı�ֵԽ�ӽ�1˵��Խ�������

    //float blendL = max(0.0, (pixelContrast / localContrast) - 0.25);
    //blendL = min(0.75, blendL); 

    vec3 rgbNW = texture(screenTexture, TexCoords + (vec2(-1.0, -1.0) * texelSize)).xyz;
    vec3 rgbNE = texture(screenTexture, TexCoords + (vec2(1.0, -1.0) * texelSize)).xyz;
    vec3 rgbSW = texture(screenTexture, TexCoords + (vec2(-1.0,  1.0) * texelSize)).xyz;
    vec3 rgbSE = texture(screenTexture, TexCoords + (vec2(1.0,  1.0) * texelSize)).xyz;
    float lumaNW = dot(rgbNW, vec3(0.299, 0.587, 0.114));
    float lumaNE = dot(rgbNE, vec3(0.299, 0.587, 0.114));
    float lumaSW = dot(rgbSW, vec3(0.299, 0.587, 0.114));
    float lumaSE = dot(rgbSE, vec3(0.299, 0.587, 0.114));

    //ȷ���߽�ķ���
    float edgeVert = 
        abs((0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE)) + 
        abs((0.50 * lumaW ) + (-1.0 * lumaM) + (0.50 * lumaE )) + 
        abs((0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE));
    float edgeHorz = 
        abs((0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW)) + 
        abs((0.50 * lumaN ) + (-1.0 * lumaM) + (0.50 * lumaS )) + 
        abs((0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE));
    bool horzSpan = edgeHorz >= edgeVert; 

    //����ʱ�Ĳ���
    vec2 texStep = horzSpan ? vec2(1.0,0.0) : vec2(0.0,1.0);
    texStep *= texelSize;
    //��������Ѱ�ұ߽��Ƿ����
    //ȷ��Ҫ��������������ض���㣬һ���Ǳ����أ�һ���Ǿ����һ�������
    vec2 pos1 = TexCoords, pos2;
    if(horzSpan){
        pos2 = (abs(lumaM - lumaN)>abs(lumaM - lumaS))? TexCoords + (vec2(0.0, -1.0) * texelSize) : TexCoords + (vec2(0.0, 1.0) * texelSize);
    } else{
        pos2 = (abs(lumaM - lumaW)>abs(lumaM - lumaE))? TexCoords + (vec2(-1.0, 0.0) * texelSize) : TexCoords + (vec2(1.0, 0.0) * texelSize);
    }

    //��ʼ���ض� �����ص����ȣ�һ����lumaM
    float lumaOther = dot(texture(screenTexture, pos2).xyz, vec3(0.299, 0.587, 0.114));

    //��ʼ��������� N������ P������
    float lumaEndN1 ,lumaEndN2, lumaEndP1,lumaEndP2;
    float countN = 0.0,countP = 0.0;
    bool nearMN= false, nearMP = false;//�߽�����ĵط����ڲ�(��Mͬɫ)�������(��Otherͬɫ)
    for(int i = 0; i < 100 ; i++)
    {   
        //�����һ�����ض�
        vec3 rgbEndN1 = texture(screenTexture, pos1 - texStep * i).xyz;
        float lumaEndN1 = dot(rgbEndN1, vec3(0.299, 0.587, 0.114));
        vec3 rgbEndN2 = texture(screenTexture, pos2 - texStep * i).xyz;
        float lumaEndN2 = dot(rgbEndN2, vec3(0.299, 0.587, 0.114));
        //������ֺ�ԭʼ���ضԲ���ϴ����ɫ������ο��ܲ��Ǿ��
        if((abs(lumaEndN1 - lumaM) > 0.15 &&  abs(lumaEndN1 - lumaOther) > 0.15) || (abs(lumaEndN2 - lumaM) > 0.15  && abs(lumaEndN2 - lumaOther) > 0.15)){
            FragColor = vec4(pow(rgbM, vec3(1.0/2.2)),1.0);
            return;
        }
        //����������ز���С����Ϊ�߽絽ͷ��
        if(abs(lumaEndN1 - lumaEndN2) < 0.1){
            //�����㶼�ӽ�lumaM����ʾ���ڲ�
            //���������, nearMN = false
            nearMN = abs(lumaM - lumaEndN2) < 0.15  && abs(lumaEndN1 - lumaM) < 0.15;
            countN = i + 1;
            break;
        }
    }

    for(int i = 0; i < 100 ; i++)
    {   
        vec3 rgbEndP1 = texture(screenTexture, pos1 + texStep * i).xyz;
        float lumaEndP1 = dot(rgbEndP1, vec3(0.299, 0.587, 0.114));
        vec3 rgbEndP2 = texture(screenTexture, pos2 + texStep * i).xyz;
        float lumaEndP2 = dot(rgbEndP2, vec3(0.299, 0.587, 0.114));
        //������ֺ�ԭʼ���ضԲ���ϴ����ɫ������ο��ܲ��Ǿ��
        if((abs(lumaEndP1 - lumaM) > 0.15 && abs(lumaEndP1 - lumaOther) > 0.15)|| (abs(lumaEndP2 - lumaM) > 0.15 && abs(lumaEndP2 - lumaOther) > 0.15)){
            FragColor = vec4(pow(rgbM, vec3(1.0/2.2)),1.0);
            return;
        }
        //����������ز���С����Ϊ�߽絽ͷ��
        if(abs(lumaEndP1 - lumaEndP2) < 0.1){
            //�����㶼�ӽ�lumaM����ʾ���ڲ�,���������
            nearMP = abs(lumaM - lumaEndP1) < 0.15  && abs(lumaEndP1 - lumaM) < 0.15;
            countP = i + 1;
            break;
        }
    }

    //ĳһ��ﵽ��������
    if(countN < 0.001 || countP < 0.001){
        FragColor = vec4(pow(rgbM, vec3(1.0/2.2)),1.0);
        return;
    }
    //���ͬ�࣬�ݲ�����
    if(nearMP == nearMN){
        FragColor = vec4(pow(rgbM, vec3(1.0/2.2)),1.0);
        return;
    }
    if(nearMN){
        vec2 pos = (countP * pos1 + countN * pos2) /  (countN + countP);
        vec3 color = texture(screenTexture, pos, 0.0).xyz;
        FragColor = vec4(pow(color, vec3(1.0/2.2)),1.0);
    } else{
        vec2 pos = (countN * pos1 + countP * pos2) /  (countN + countP);
        vec3 color = texture(screenTexture, pos, 0.0).xyz;
        FragColor = vec4(pow(color, vec3(1.0/2.2)),1.0); 
    }





}