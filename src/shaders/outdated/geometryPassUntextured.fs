#version 450 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gAlbedoSpec;
layout (location = 2) out vec4 gNormal;

in DATA
{
	vec3 position;
	vec3 normal;
    vec4 color;
} fs_in;

void main()
{    
	gPosition = vec4(fs_in.position, 1);
    gAlbedoSpec.rgb = fs_in.color.gba;
    gAlbedoSpec.a = 1;
	gNormal = vec4(fs_in.normal, 1);
}
