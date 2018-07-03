#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 proj;
uniform mat4 view;

out vec3 localPos;

void main()
{
    localPos = aPos;

    gl_Position = (proj * vec4(mat3(view) * aPos, 1.0)).xyww;
}
