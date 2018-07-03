#version 330 core

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
    	vec3 N;
} fs_in;

struct PointLight
{
	vec4 position;
	vec4 color3_rad1;
};

uniform float numLights = 0;
layout(std140) uniform Lights
{
	PointLight pointLights[MAX_LIGHTS];
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

vec3 calc_light_pbr(vec3 viewDir, vec3 fragPos, vec3 lightPos, float NdotV, 
                    vec3 Norm, vec3 albedo, vec3 F0, vec3 lightColor, float roughness, float metallic)
{
    vec3 L = normalize(lightPos - fragPos);
    vec3 H = normalize(viewDir + L);
    float NdotL = max(dot(Norm, L), 0.0f);

    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(Norm, H, roughness);   
    float G   = GeometrySmith(NdotV, NdotL, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0f), F0);
       
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * NdotV * NdotL + 0.0001f;
    vec3 specular = nominator / denominator; 
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;     

    return (kD * albedo / PI + specular) * radiance * NdotL;
}

mat3 genTBN()
{
    vec3 dFragX  = dFdx(fs_in.fragPos);
    vec3 dFragY  = dFdy(fs_in.fragPos);
    vec2 dTexX = dFdx(fs_in.texCoord);
    vec2 dTexY = dFdy(fs_in.texCoord);

    vec3 N  = normalize(-fs_in.N); 
    vec3 T  = normalize(dFragX*dTexY.t - dFragY*dTexX.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return TBN;
}

// float calc_shadow(vec4 lightFragPos, vec3 normal)
// {
//     if(dot(lightPos, normal) > 0) return 1.0f;
// 
//     vec3 projCoords = lightFragPos.xyz ;
//     projCoords = projCoords * 0.5 + 0.5;
//     float closest = texture(depthShadowMap, projCoords.xy).r;
//     float current = projCoords.z;
// 
//     float shadow = 0.0;
//     vec2 texelSize = 1.0 / textureSize(depthShadowMap, 0);
//     for(int x = -1; x <= 1; ++x)
//     {
//         for(int y = -1; y <= 1; ++y)
//         {
//             float pcfDepth = texture(depthShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
//             shadow += current > pcfDepth ? 1.0 : 0.0;        
//         }    
//     }
//     shadow /= 9.0;
// 
//     return shadow;
// }

// const uint rsmNumSamples = 100u;
// const float rsmNumSamplesInverse = 1 / float(rsmNumSamples);
// const float indirectIntensity = 300.5f;
// float RadicalInverse_VdC(uint bits) 
// {
//     bits = (bits << 16u) | (bits >> 16u);
//     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
//     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
//     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
//     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
//     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
// }
// vec2 Hammersley(uint i, float N_inv)
// {
//     return vec2(float(i) * float(N_inv), RadicalInverse_VdC(i));
// }  
// vec3 calc_indirect_light(vec3 lightFragPos, vec3 normal)
// {
//     vec2 projCoords = lightFragPos.xy;
//     projCoords = projCoords * 0.5 + 0.5;
// 
//     vec3 indirect = vec3(0.0f);
//     float sampleRadius = 0.6f;
//     
//     for (uint i = 0u; i < rsmNumSamples; ++i)
//     {
// 	vec2 rnd = Hammersley(i, rsmNumSamplesInverse);
// 	projCoords = (projCoords + sampleRadius * rnd).xy;
//         
//         vec3 vplPos = texture(positionShadowMap, projCoords).rgb;
//         vec3 vplNorm = texture(normalShadowMap, projCoords).rgb;
//         vec3 vplFlux = texture(fluxShadowMap, projCoords).rgb;
// 
// 	vec3 res = vplFlux * 
// 		   ( (max(0, dot(vplNorm, lightFragPos - vplPos)) *
// 		      max(0, dot(normal, vplPos - lightFragPos))) /
// 		      pow(length(lightFragPos - vplPos), 4) );
// 
// 	res *= rnd.x * rnd.x;
//         indirect += res;
//     }
//     
//     return clamp(indirect * indirectIntensity, 0.0f, 1.0f);
// }

//vec3 test_shadow(vec4 lightFragPos, vec3 normal)
//{
//    vec3 projCoords = lightFragPos.xyz ;
//    projCoords = projCoords * 0.5 + 0.5;
//    return texture(normalShadowMap, projCoords.xy).rgb;
//}

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
    vec3 N = int(fs_in.layer1.y) == 255 ? -normalize(fs_in.N) : normalize(TBN * tangentNormal);

    vec3 F0 = vec3(0.04f); 
    F0 = mix(F0, albedo, metallic); 

    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    float NdotV = max(dot(N, viewDir), 0.0);

    vec3 Lo = vec3(0.0f);
    for(uint i = 0u; i < lightsInCell.a; ++i)
    {
	uint lightIndex = lightsInCell[i]; 
	PointLight light = pointLights[lightIndex];
	Lo += calc_light_pbr( viewDir, fs_in.fragPos, light.position.xyz, NdotV, 	         N, albedo, F0, light.color3_rad1.xyz, roughness, metallic);
    }

    vec3 R = reflect(-viewDir, N);

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

