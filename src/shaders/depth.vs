#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 layer1;
layout(location = 3) in vec4 layer2;
layout(location = 4) in vec2 texCoord;
//layout(location = 5) in vec3 tangent;
layout(location = 6) in mat4 modelMatrix;
 
uniform mat4 PV;

void main()
{
    gl_Position = PV * modelMatrix * vec4(position, 1.0);
}  
