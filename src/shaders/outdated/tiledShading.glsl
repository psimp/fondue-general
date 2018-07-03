#version 430 

//external
MAX_WORK_GROUP_SIZE
SCREEN_WIDTH
SCREEN_HEIGHT
MAX_LIGHTS
//external end

#define DEBUG_TILING 0  // Set to 1 to see tile frustum borders
#define LIGHT_GRID_MAX_DIM_X ((SCREEN_WIDTH + MAX_WORK_GROUP_SIZE - 1) / MAX_WORK_GROUP_SIZE)
#define LIGHT_GRID_MAX_DIM_Y ((SCREEN_HEIGHT + MAX_WORK_GROUP_SIZE - 1) / MAX_WORK_GROUP_SIZE)

layout(local_size_x = MAX_WORK_GROUP_SIZE, local_size_y = MAX_WORK_GROUP_SIZE) in;

layout(binding = 28, rgba32f) uniform writeonly image2D finalImage;

struct PointLight
{
	vec4 position;
	vec4 color3_rad1;
};

uniform float numLights = 0;
layout(binding = 0, std140) uniform Lights
{
	PointLight pointLights[MAX_LIGHTS];
};

uniform sampler2D normalTexture;
uniform sampler2D albedoTexture;
uniform sampler2D positionTexture;
uniform sampler2D tangentTexture;
uniform sampler2D biTangentTexture;
uniform sampler2D TSNormalTexture;

uniform mat4 projectionMatrix;
uniform mat4 projectionMatrixInverse;
uniform mat4 viewMatrix;
uniform vec3 viewPos;

shared uint pointLightIndex[MAX_LIGHTS];
shared uint pointLightCount = 0;

const float metallic = 0.5f;
const float roughness = 0.5f;
const float ao = 1.0f; 

const float PI = 3.14159265359; 

vec4 unProject(vec4 v)
{
    v = projectionMatrixInverse * v; // Define outside of shader!
	v /= v.w;
	return v;
}
vec4 CreatePlane( vec4 b, vec4 c )
{ 
    vec4 normal;
    normal.xyz = normalize(cross( b.xyz, c.xyz ));
    normal.w = 0;
    return normal;
}

float GetSignedDistanceFromPlane( vec4 p, vec4 eqn )
{
    return dot( eqn.xyz, p.xyz );
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------

vec4 CalcPointLight(vec3 TangentViewPos, vec3 TangentFragPos, vec3 TangentLightPos,
                    vec3 normal, vec3 Diffuse, float radius, float cutoff, vec3 lightColor)
{
    vec3 lightDist = TangentLightPos - TangentFragPos;
    vec3 viewDir  = normalize(TangentViewPos - TangentFragPos);
    // diffuse
    vec3 lightDir = normalize(lightDist);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * Diffuse * lightColor;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = lightColor * spec; // * Specular;
    // attenuation
    float distance = length(lightDist);
    float LinearLight = 0.9, QuadraticLight = 4.8;
    float attenuation = 1.0 / (1.0 + LinearLight * distance + QuadraticLight * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;

    return vec4(diffuse + specular, 1.0);
}

vec3 calc_light_pbr(vec3 viewDir, vec3 fragPos, vec3 lightPos, vec3 Norm, vec3 albedo, vec3 F0, vec3 lightColor)
{
    vec3 L = normalize(lightPos - fragPos);
    vec3 H = normalize(viewDir + L);
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(Norm, H, roughness);   
    float G   = GeometrySmith(Norm, viewDir, L, roughness);      
    vec3 F    = fresnelSchlick(clamp(dot(H, viewDir), 0.0, 1.0), F0);
       
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(Norm, viewDir), 0.0) * max(dot(Norm, L), 0.0);
    vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
    
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;     

    // scale light by NdotL
    float NdotL = max(dot(Norm, L), 0.0);

    return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main()
{
	if(gl_LocalInvocationIndex == 0)
		pointLightCount = 0;

	uint minX = MAX_WORK_GROUP_SIZE * gl_WorkGroupID.x;
	uint minY = MAX_WORK_GROUP_SIZE * gl_WorkGroupID.y;
	uint maxX = MAX_WORK_GROUP_SIZE * (gl_WorkGroupID.x + 1);
	uint maxY = MAX_WORK_GROUP_SIZE * (gl_WorkGroupID.y + 1);

	vec4 tileCorners[4];
	tileCorners[0] = unProject(vec4( (float(minX)/SCREEN_WIDTH) * 2.0f - 1.0f, (float(minY)/SCREEN_HEIGHT) * 2.0f - 1.0f, 1.0f, 1.0f));
	tileCorners[1] = unProject(vec4( (float(maxX)/SCREEN_WIDTH) * 2.0f - 1.0f, (float(minY)/SCREEN_HEIGHT) * 2.0f - 1.0f, 1.0f, 1.0f));
	tileCorners[2] = unProject(vec4( (float(maxX)/SCREEN_WIDTH) * 2.0f - 1.0f, (float(maxY)/SCREEN_HEIGHT) * 2.0f - 1.0f, 1.0f, 1.0f));
	tileCorners[3] = unProject(vec4( (float(minX)/SCREEN_WIDTH) * 2.0f - 1.0f, (float(maxY)/SCREEN_HEIGHT) * 2.0f - 1.0f, 1.0f, 1.0f));

	vec4 frustum[4];
	for(int i = 0; i < 4; i++)
		frustum[i] = CreatePlane(tileCorners[i],tileCorners[(i+1) & 3]);

	barrier();

	int threadsPerTile = MAX_WORK_GROUP_SIZE*MAX_WORK_GROUP_SIZE;
	for (uint i = 0; i < numLights; i+= threadsPerTile)
    {
        uint il = gl_LocalInvocationIndex + i;
        if (il < numLights)
        {
            PointLight light = pointLights[il];

            vec4 viewLightPos = viewMatrix * light.position;
            float r = light.color3_rad1.w;

            if( ( GetSignedDistanceFromPlane( viewLightPos, frustum[0] ) < r ) &&
                ( GetSignedDistanceFromPlane( viewLightPos, frustum[1] ) < r ) &&
                ( GetSignedDistanceFromPlane( viewLightPos, frustum[2] ) < r ) &&
                ( GetSignedDistanceFromPlane( viewLightPos, frustum[3] ) < r) )

                {
                    uint id = atomicAdd(pointLightCount, 1);
                    pointLightIndex[id] = il;
                }
        }
    }

	barrier();

	ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
	vec2 texCoord = vec2(pixelPos.x / SCREEN_WIDTH, pixelPos.y / SCREEN_HEIGHT);

	vec3 albedo = texture(albedoTexture, texCoord).rgb;
	vec3 FragPos = texture(positionTexture, texCoord).rgb;
    vec3 T = texture(tangentTexture, texCoord).rgb;
    vec3 B = texture(biTangentTexture, texCoord).rgb;
	vec3 N = texture(normalTexture, texCoord).rgb;
    mat3 TBN = transpose(mat3(T, B, N));    

    vec3 TangentViewPos  = TBN * viewPos;
    vec3 TangentFragPos  = TBN * FragPos;

    vec3 TSNormal = texture(TSNormalTexture, vec2(texCoord)).xyz;
    TSNormal = normalize(TSNormal * 2 - 1);

    vec3 TangentViewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic); 

    vec3 Lo = vec3(0.0f);
	for(uint i = 0; i < pointLightCount; ++i)
	{
		uint lightIndex = pointLightIndex[i]; 
		PointLight light = pointLights[lightIndex]; 
        vec3 TangentLightPos = TBN * light.position.xyz;
		Lo += calc_light_pbr(TangentViewDir, TangentFragPos, TangentLightPos, 
                                TSNormal, albedo, F0, vec3(light.color3_rad1.xyz));
	}

    vec3 color = vec3(0.03) * albedo * ao + Lo;

	barrier();

	imageStore(finalImage, pixelPos, vec4(color, 1.0f));

#if DEBUG_TILING == 1
	if (gl_LocalInvocationID.x == 0 || gl_LocalInvocationID.y == 0 || gl_LocalInvocationID.x == MAX_WORK_GROUP_SIZE || gl_LocalInvocationID.y == MAX_WORK_GROUP_SIZE)
		imageStore(finalImage, pixelPos, vec4(.5f, .5f, .5f, 1.0f));
#endif
}

