#version 330 core
#define M_PI 3.1415926535897932384626433832795
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 view;

out vec3 ourColor;
out vec2 TexCoord;

uniform float time;

mat4 roty(float angle) {
    return mat4(cos(angle), 0, sin(angle), 0,
                0, 1, 0, 0,
                -sin(angle), 0, cos(angle), 0,
                0, 0, 0, 1);
}

void main()
{ 
    gl_Position = projection * view * roty(time * 2) * vec4(aPos,  1.0); 
    TexCoord = aTexCoord;
}
