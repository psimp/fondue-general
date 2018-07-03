#version 450 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D image;

void main()
{
    vec2 color = texture(image, texCoord).rg;
    FragColor = vec4(color, 0, 1);
}
