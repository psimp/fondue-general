#version 330 core 

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gFlux;

in DATA
{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
    vec4 layer1;
} fs_in;

uniform sampler2DArray textures;

void main()
{
   gPosition = vec4(fs_in.fragPos, 1);
   gNormal = vec4(fs_in.normal, 1);
   gFlux = int(fs_in.layer1.x) == 255 ? vec4(0.7f, 0.1f, 0.1f, 1.0f) : texture(textures, vec3(fs_in.texCoord, fs_in.layer1.x));
}

