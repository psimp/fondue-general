#version 330 core

layout(location = 0) in vec3 position;
layout(location = 6) in mat4 modelMatrix;
layout(location = 10) in vec3 colour;

out DATA
{
	vec3 colour;
} vs_out;

uniform mat4 ProjectionView;

void main()
{
	vec4 modelPosition = modelMatrix * vec4(position, 1.0f);
	
	vs_out.colour = colour;

	gl_Position = ProjectionView * modelPosition;
}
