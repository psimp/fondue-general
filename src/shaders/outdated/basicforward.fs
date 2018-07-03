#version 450 core

out vec4 FragColor;

in DATA
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	flat vec4 layer;
    vec3 tangent;
    vec3 biTangent;
} fs_in;

uniform sampler2DArray textures;

void main()
{    
    FragColor = vec4(texture(textures, vec3(fs_in.texCoord, fs_in.layer.x);
}
