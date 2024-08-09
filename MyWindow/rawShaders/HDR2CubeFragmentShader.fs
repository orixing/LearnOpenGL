#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform sampler2D envHDRTex;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

vec3 ACESToneMapping(vec3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

void main()
{       
    vec2 uv = SampleSphericalMap(normalize(localPos)); // make sure to normalize localPos
    vec3 color = texture(envHDRTex, uv).rgb;

    color = ACESToneMapping(color, 1.0);

    color.x = min(1.0,color.x);
    color.y = min(1.0,color.y);
    color.z = min(1.0,color.z);

    FragColor = vec4(color, 1.0);
    //FragColor = vec4(1.0);
}
