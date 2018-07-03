#version 430 core

layout (location = 0) in vec3 position;

out vec3 localPos;

uniform mat4 PV;

void main()
{
    localPos = position;
    gl_Position = PV * vec4(position, 1.0);
}
