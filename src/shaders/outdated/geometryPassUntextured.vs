#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec4 color;
layout(location = 5) in mat4 modelMatrix;

out DATA
{
	vec3 position;
	vec3 normal;
    vec4 color;
} vs_out;

layout(std140) uniform RunUniforms
{
    uniform mat4 ProjectionView;
};

void main()
{
	vec4 modelPosition = modelMatrix * vec4(position, 1.0);
	gl_Position = ProjectionView * modelPosition;
	vs_out.position = modelPosition.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vs_out.normal = normalize(normalMatrix * normal);

    vs_out.color = color;
}
