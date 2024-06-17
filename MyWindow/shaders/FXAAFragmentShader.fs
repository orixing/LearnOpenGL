#version 330 core

// 输入的包含渲染场景的纹理
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

    //最大亮度和最小亮度
    float lumaMin = min(lumaM, min(min(lumaN, lumaE), min(lumaS, lumaW)));
    float lumaMax = max(lumaM, max(max(lumaN, lumaE), max(lumaS, lumaW)));
    float localContrast = lumaMax - lumaMin;

    //差小于阈值，不做处理
    if(localContrast < max(FXAA_THRESHOLD, FXAA_EDGE_THRESHOLD_MIN * lumaMax)){
        FragColor = vec4(rgbM,1.0);
        return;
    }

    float pixelContrast = abs(lumaM - 0.25*(lumaN+lumaW+lumaS+lumaE));
    //pixel contrast与local contrast的比值越接近1说明越像是噪点

    float blendL = max(0.0, (pixelContrast / localContrast) - 0.25);
    blendL = min(0.75, blendL); 

  

    vec3 rgbNW = texture(screenTexture, TexCoords + (vec2(-1.0, -1.0) * texelSize)).xyz;
    vec3 rgbNE = texture(screenTexture, TexCoords + (vec2(1.0, -1.0) * texelSize)).xyz;
    vec3 rgbSW = texture(screenTexture, TexCoords + (vec2(-1.0,  1.0) * texelSize)).xyz;
    vec3 rgbSE = texture(screenTexture, TexCoords + (vec2(1.0,  1.0) * texelSize)).xyz;
    float lumaNW = dot(rgbNW, vec3(0.299, 0.587, 0.114));
    float lumaNE = dot(rgbNE, vec3(0.299, 0.587, 0.114));
    float lumaSW = dot(rgbSW, vec3(0.299, 0.587, 0.114));
    float lumaSE = dot(rgbSE, vec3(0.299, 0.587, 0.114));

    //确定边界的方向
    float edgeVert = 
        abs((0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE)) + 
        abs((0.50 * lumaW ) + (-1.0 * lumaM) + (0.50 * lumaE )) + 
        abs((0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE));
    float edgeHorz = 
        abs((0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW)) + 
        abs((0.50 * lumaN ) + (-1.0 * lumaM) + (0.50 * lumaS )) + 
        abs((0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE));
    bool horzSpan = edgeHorz >= edgeVert; 

    vec2 texStep = horzSpan ? vec2(1.0,0.0) : vec2(0.0,1.0);
    texStep *= texelSize;
    //正反方向寻找边界是否结束
    //确定要向两侧遍历的像素对起点
    vec2 pos1 = TexCoords, pos2;
    if(horzSpan){
        pos2 = (abs(lumaM - lumaN)>abs(lumaM - lumaS))? TexCoords + (vec2(0.0, -1.0) * texelSize) : TexCoords + (vec2(0.0, 1.0) * texelSize);
    } else{
        pos2 = (abs(lumaM - lumaW)>abs(lumaM - lumaE))? TexCoords + (vec2(-1.0, 0.0) * texelSize) : TexCoords + (vec2(1.0, 0.0) * texelSize);
    }

    //开始向两侧遍历
    float lumaEndN1 ,lumaEndN2, lumaEndP1,lumaEndP2;
    float countN = 0.0,countP = 0.0;
    bool nearMN= false, nearMP = false;//边界结束的地方是内侧还是外侧
    for(int i = 0; i < 20 ; i++)
    {   
        vec3 rgbEndN1 = texture(screenTexture, pos1 + texStep * i).xyz;
        float lumaEndN1 = dot(rgbEndN1, vec3(0.299, 0.587, 0.114));
        vec3 rgbEndN2 = texture(screenTexture, pos2 + texStep * i).xyz;
        float lumaEndN2 = dot(rgbEndN2, vec3(0.299, 0.587, 0.114));
        //如果两个像素差距较小，认为边界到头了，如果原采样点lumaM和对应的lumaEnd1差距大，也认为边界到头了
        if(abs(lumaEndN1 - lumaEndN2) < 0.25 || abs(lumaEndN1 - lumaM)>0.25){
            //两个点都接近lumaM，表示在内侧
            if(abs(lumaM - lumaEndN2) < 0.25  && abs(lumaEndN1 - lumaM) < 0.25 ){
                nearMN = true;
            }//否则在外侧
            break;
        }
        countN +=1.0;
    }

    for(int i = 0; i < 20 ; i++)
    {   
        vec3 rgbEndP1 = texture(screenTexture, pos1 - texStep * i).xyz;
        float lumaEndP1 = dot(rgbEndP1, vec3(0.299, 0.587, 0.114));
        vec3 rgbEndP2 = texture(screenTexture, pos2 - texStep * i).xyz;
        float lumaEndP2 = dot(rgbEndP2, vec3(0.299, 0.587, 0.114));
        //如果两个像素差距较小，认为边界到头了，如果原采样点lumaM和对应的lumaEnd1差距大，也认为边界到头了
        if(abs(lumaEndP1 - lumaEndP2) < 0.25 || abs(lumaEndP1 - lumaM)>0.25){
            //两个点都接近lumaM，表示在内侧
            if(abs(lumaM - lumaEndP1) < 0.25  && abs(lumaEndP1 - lumaM) < 0.25 ){
                nearMP = true;
            }//否则在外侧
            break;
        }
        countP +=1.0;
    }

    float totalL = countN + countP + 1.0;

    //如果同侧，暂不处理
    if(nearMP == nearMN){
        FragColor = vec4(rgbM,1.0);
        return;
    }
    if(nearMN){
        vec2 pos = ((countP + 1.0) * pos1 + (countN + 1.0) * pos2)/  (totalL+1.0);
        FragColor = vec4(textureLod(screenTexture, pos, 0.0).xyz,1.0);
    } else{
        vec2 pos = ((countN + 1.0) * pos1 + (countP + 1.0) * pos2)/  (totalL+1.0);
        FragColor = vec4(textureLod(screenTexture, pos, 0.0).xyz,1.0);
    }





}