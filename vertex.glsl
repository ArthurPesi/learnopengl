#version 330 core
#define M_PI 3.1415926535897932384626433832795
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 projection;
uniform vec3 player;

out vec3 ourColor;
out vec2 TexCoord;

uniform float time;

void main()
{ 
    gl_Position = projection * vec4(aPos - player,  1.0); 
    ourColor = aColor;
    TexCoord = aTexCoord;
}
