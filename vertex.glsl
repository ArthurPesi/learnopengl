#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
uniform vec2 player;
uniform vec2 floating;
out vec3 ourColor;

void main()
{ 
    gl_Position = vec4(aPos.x - player.x, floating.y + aPos.y - player.y, 0.0, 1.0); 
    ourColor = gl_Position.xyz;
}
