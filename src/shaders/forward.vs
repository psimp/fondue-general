#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 layer1;
layout(location = 3) in vec4 layer2;
layout(location = 4) in vec2 texCoord;
//layout(location = 5) in vec3 tangent;
layout(location = 6) in mat4 modelMatrix;

out DATA
{
	vec3 fragPos;
	vec2 texCoord;
	flat vec4 layer1;
    	flat vec4 layer2;
    	vec3 normal;
} vs_out;

uniform mat4 ProjectionView;

void main()
{
	vec4 modelPosition = modelMatrix * vec4(position, 1.0f);

    	vs_out.normal = mat3(modelMatrix) * normal;
	vs_out.fragPos = modelPosition.xyz;
	vs_out.texCoord = texCoord;
	vs_out.layer1 = layer1;
	vs_out.layer2 = layer2;

	gl_Position = ProjectionView * modelPosition;
}
