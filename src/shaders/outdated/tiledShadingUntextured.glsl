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

layout(binding = 21, rgba16f) uniform writeonly image2D finalImage;

struct PointLight
{
	vec4 position;
	vec4 color3_rad1;
};

layout (std140) uniform RunUniforms
{
    mat4 viewMatrix;
    vec4 viewPos_nLights;
};

layout(binding = 0, std140) uniform Lights
{
	PointLight pointLights[MAX_LIGHTS];
};

uniform sampler2D normalTexture;
uniform sampler2D albedoTexture;
uniform sampler2D positionTexture;

uniform mat4 projectionMatrix;
uniform mat4 projectionMatrixInverse;

shared uint pointLightIndex[MAX_LIGHTS];
shared uint pointLightCount = 0;

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

vec3 CalcPointLight(vec3 fragPos, vec3 normal, vec3 lightPos, vec3 Diffuse, float radius, float cutoff, vec3 lightColor)
{
    vec3 viewPos = viewPos_nLights.xyz;

    vec3 lightDist = lightPos - fragPos;
    vec3 viewDir  = normalize(viewPos - fragPos);
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

    return diffuse + specular;
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

    float numLights = viewPos_nLights.w;

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

	ivec2 pixelPosition = ivec2(gl_GlobalInvocationID.xy);
	vec2 texCoord = vec2(pixelPosition.x / SCREEN_WIDTH, pixelPosition.y / SCREEN_HEIGHT);

	vec3 Diffuse = texture(albedoTexture, texCoord).rgb;
	vec3 FragPos = texture(positionTexture, texCoord).rgb;
    vec3 Normal = texture(normalTexture, texCoord).rgb;

    float ambientScale = 0.1;
	vec3 color = ambientScale * Diffuse; 

	for(uint i = 0; i < pointLightCount; ++i)
	{
		uint lightIndex = pointLightIndex[i]; 
		PointLight light = pointLights[lightIndex]; 
		color += CalcPointLight(FragPos, Normal, light.position.xyz, Diffuse, 2, 0.5f, light.color3_rad1.xyz);
	}

	barrier();

	imageStore(finalImage, pixelPosition, vec4(color, 1.0f));

#if DEBUG_TILING == 1
	if (gl_LocalInvocationID.x == 0 || gl_LocalInvocationID.y == 0 || gl_LocalInvocationID.x == MAX_WORK_GROUP_SIZE || gl_LocalInvocationID.y == MAX_WORK_GROUP_SIZE)
		imageStore(finalImage, pixelPosition, vec4(.5f, .5f, .5f, 1.0f));
#endif
}

