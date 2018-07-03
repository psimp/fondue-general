#version 450 core

//external
SCREEN_WIDTH
SCREEN_HEIGHT
MAX_VIEWSPACE_MESHES
NUM_SAMPLES
MAX_LIGHTS
//external end

out vec4 FragColor;

in DATA
{
	vec3 fragPos;
	vec2 texCoord;
	flat vec4 layer1;
	flat vec4 layer2;
    	vec3 normal;
} fs_in;

struct PointLight
{
	vec4 position;
	vec4 color3_rad1;
};

uniform float numLights = 0;
layout(std140) uniform Lights
{
	PointLight pointLights[int(MAX_LIGHTS)];
};

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT; 

uniform sampler2DArray textures3C;
uniform sampler2DArray textures1C;

uniform usampler2D lightCullResults;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 viewPos;

const float PI = 3.14159265359; 

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float NdotH = max(dot(N, H), 0.0f);
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH2 = NdotH*NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}

float GeometrySchlickGGX(float cosine, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float denom = cosine * (1.0 - k) + k;

    return cosine / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float SchlickFresnel(float cosTheta)
{
    float x = clamp(1.0-cosTheta, 0.0, 1.0);
    float x2 = x*x;
    return x2*x2*x;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * SchlickFresnel(cosTheta);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * SchlickFresnel(cosTheta);
}   

mat3 genTBN()
{
    vec3 dFragX  = dFdx(fs_in.fragPos);
    vec3 dFragY  = dFdy(fs_in.fragPos);
    vec2 dTexX = dFdx(fs_in.texCoord);
    vec2 dTexY = dFdy(fs_in.texCoord);

    vec3 N  = normalize(-fs_in.normal); 
    vec3 T  = normalize(dFragX*dTexY.t - dFragY*dTexX.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return TBN;
}

const float MAX_REFLECTION_LOD = 4.0;
void main()
{    
    vec2 cellIndex = gl_FragCoord.xy / vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
    uvec4 lightsInCell = texture(lightCullResults, cellIndex);

    vec3 albedo = int(fs_in.layer1.x) == 255 ? vec3(0.7f) : pow(texture(textures3C, vec3(fs_in.texCoord, fs_in.layer1.x)).rgb, vec3(2.2f));
    vec3 tangentNormal = int(fs_in.layer1.y) == 255 ? vec3(0.5f) : texture(textures3C, vec3(fs_in.texCoord, fs_in.layer1.y)).rgb * 2.0 - 1.0;
    float roughness = int(fs_in.layer1.w) == 255 ? 0.9f : texture(textures1C, vec3(fs_in.texCoord, fs_in.layer1.z)).r;
    float metallic = int(fs_in.layer1.z) == 255 ? 0.01f : texture(textures1C, vec3(fs_in.texCoord, fs_in.layer1.w)).r;
    float ao = int(fs_in.layer2.x) == 255 ? 0.7f : texture(textures1C, vec3(fs_in.texCoord, fs_in.layer2.x)).r;

    mat3 TBN = genTBN();
    vec3 N = int(fs_in.layer1.y) == 255 ? -normalize(fs_in.normal) : normalize(TBN * tangentNormal);

    vec3 F0 = vec3(0.04f); 
    F0 = mix(F0, albedo, metallic); 

    vec3 vD = normalize(viewPos - fs_in.fragPos);
    float NdotV = max(dot(N, vD), 0.0);

    vec3 Lo = vec3(0.0f);
    for(uint i = 0u; i < 0u; ++i)
    {
//	uint lightIndex = lightsInCell[i]; 
//	PointLight light = pointLights[lightIndex];
//	vec3 lP = light.position.xyz; 
//	vec3 fP = fs_in.fragPos; 
//	vec3 lC = light.color3_rad1.xyz;
//
//        float d = length(lP - fP);
//        vec3 L = (lP - fP)/d;
//        vec3 H = normalize(vD + L);
//        float NdotL = max(dot(N, L), 0.0f);
//
//
//        float attenuation = 1.0 / (d * d);
//        vec3 radiance = lC * attenuation;
//
//        float NDF = DistributionGGX(N, H, roughness);   
//        float G   = GeometrySmith(NdotV, NdotL, roughness);      
//        vec3 F    = fresnelSchlick(max(dot(H, vD), 0.0f), F0);
//           
//        vec3 nominator    = NDF * G * F; 
//        float denominator = 4 * NdotV * NdotL + 0.0001f;
//        vec3 specular = nominator / denominator; 
//        
//        vec3 kS = F;
//        vec3 kD = vec3(1.0) - kS;
//        kD *= 1.0 - metallic;     
//
//        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 R = reflect(-vD, N);

    vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
      
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * albedo;
      
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;   
    vec2 envBRDF  = texture(brdfLUT, vec2(NdotV, roughness)).rg;
    vec3 specular = prefilteredColor * (F * 0.5 * (envBRDF.x + envBRDF.y));
      
    vec3 ambient = (kD * diffuse + specular) * ao; 

    vec3 color = ambient;

    // Tonemapping
    //vec3 x = max(vec3(0.0f), color - 0.004f);
    //vec3 RETURN = (8.2 * x) / (8.2 * x + 2.36); 

    color = color / (color + vec3(1.0));
    vec3 RETURN = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(RETURN, 1.0f);
}

