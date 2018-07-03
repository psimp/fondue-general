#version 330 core

out vec4 FragColor;

in DATA
{
	vec3 colour;
} fs_in;

void main()
{    
       FragColor = vec4(fs_in.colour, 1.0f);
}

