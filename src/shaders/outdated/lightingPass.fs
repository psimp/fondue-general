#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gNormal;

struct Light {
    vec4 Position;
    vec4 Color;
};

const float LinearLight = 0.9;
const float QuadraticLight = 1.8;

const int NR_LIGHTS = 1;
layout (std140) uniform Lights 
{
	Light lights[NR_LIGHTS];
};

uniform vec3 viewPos;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, texCoord).rgb;
    vec3 Normal = texture(gNormal, texCoord).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, texCoord).rgb;
    float Specular = texture(gAlbedoSpec, texCoord).a;
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position.xyz - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color.rgb;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);
        vec3 specular = lights[i].Color.rgb * spec * Specular;
        // attenuation
        float distance = length(lights[i].Position.xyz - FragPos);
        float attenuation = 1.0 / (1.0 + LinearLight * distance + QuadraticLight * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;        
    }
    FragColor = vec4(lighting, 1.0);
}
