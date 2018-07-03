#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec4 layer;
layout(location = 4) in vec3 tangent;
layout(location = 5) in mat4 modelMatrix;

out DATA
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	flat vec4 layer;
    vec3 tangent;
    vec3 biTangent;
} vs_out;

uniform mat4 ProjectionView;

void main()
{
	vec4 modelPosition = modelMatrix * vec4(position, 1.0);
	gl_Position = ProjectionView * modelPosition;
	vs_out.position = modelPosition.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N,T); 

	vs_out.normal = N;
    vs_out.tangent = T;
    vs_out.biTangent = B;

	vs_out.texCoord = texCoord;
	vs_out.layer = layer;
}
