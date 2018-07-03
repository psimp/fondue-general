#version 450 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gAlbedoSpec;
layout (location = 2) out vec4 gNormal;
layout (location = 4) out vec4 gTangent;
layout (location = 5) out vec4 gBiTangent;
layout (location = 6) out vec4 gTSNormal;

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
	gPosition = vec4(fs_in.position, 1);
	gNormal = vec4(fs_in.normal, 1);
    gTangent = vec4(fs_in.tangent, 1);
    gBiTangent = vec4(fs_in.biTangent, 1);

    gAlbedoSpec.rgb = texture(textures, vec3(fs_in.texCoord, fs_in.layer.x)).rgb;
    gTSNormal.rgb = texture(textures, vec3(fs_in.texCoord, fs_in.layer.y)).rgb;

    gTSNormal.a = 1.0;
	gAlbedoSpec.a = 1.0;
}
