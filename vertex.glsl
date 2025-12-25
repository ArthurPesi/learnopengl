#version 330 core
#define M_PI 3.1415926535897932384626433832795
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform vec2 player;
uniform float rot;
uniform float scale;

out vec3 ourColor;
out vec2 TexCoord;

vec2 rotate2d(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, s, -s, c);
    return m * v;
}

void main()
{ 
    gl_Position = vec4((rotate2d(aPos.xy, rot) - player) / scale, 0.0, aPos.z); 
    ourColor = aColor;
    TexCoord = aTexCoord;
}
