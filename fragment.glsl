#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float time;
out vec4 FragColor;

void main()
{ 
    FragColor = mix(texture(texture1, TexCoord),
                    texture(texture2, TexCoord), (sin(time/1.5) / 2.0) + 0.5);
}
