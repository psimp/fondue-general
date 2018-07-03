#version 430 

//external
SCREEN_WIDTH
SCREEN_HEIGHT
MAX_VIEWSPACE_MESHES
NUM_SAMPLES
TILE_SIZE
MAX_LIGHTS
//external end

layout(location = 0) out uvec4 Light1_4;
layout(location = 1) out uvec4 Light5_8;
layout(location = 2) out uvec4 Light9_12;

in vec3 FragPos;

struct PointLight
{
	vec4 position;
	vec4 color3_rad1;
};

uniform int numLights = 0;
layout(binding = 0, std140) uniform Lights
{
	PointLight pointLights[MAX_LIGHTS];
};

uniform mat4 projectionMatrixInverse;
uniform mat4 viewMatrix;

vec4 unProject(vec4 v)
{
    v = projectionMatrixInverse * v; 
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

void main()
{
    Light1_4 = uvec4(0);
    ivec2 cTile = ivec2(gl_FragCoord.xy);

    uint minX = cTile.x;
    uint minY = cTile.y;
    uint maxX = (cTile.x + 1);
    uint maxY = (cTile.y + 1);
    
    vec4 tileCorners[4];
    tileCorners[0] = unProject(vec4( (float(minX)/TILE_SIZE) * 2.0f - 1.0f, (float(minY)/TILE_SIZE) * 2.0f - 1.0f, 1.0f, 1.0f));
    tileCorners[1] = unProject(vec4( (float(maxX)/TILE_SIZE) * 2.0f - 1.0f, (float(minY)/TILE_SIZE) * 2.0f - 1.0f, 1.0f, 1.0f));
    tileCorners[2] = unProject(vec4( (float(maxX)/TILE_SIZE) * 2.0f - 1.0f, (float(maxY)/TILE_SIZE) * 2.0f - 1.0f, 1.0f, 1.0f));
    tileCorners[3] = unProject(vec4( (float(minX)/TILE_SIZE) * 2.0f - 1.0f, (float(maxY)/TILE_SIZE) * 2.0f - 1.0f, 1.0f, 1.0f));
    
    vec4 frustum[4];
    for(int i = 0; i < 4; i++)
    	frustum[i] = CreatePlane(tileCorners[i],tileCorners[(i+1) & 3]);

    uint lc = 0;
    for (int i = 0; i < numLights; i++)
    {
        PointLight light = pointLights[i];

        vec4 viewLightPos = viewMatrix * light.position;
        float r = light.color3_rad1.w;

        if( ( lc < 3 ) &&
            ( GetSignedDistanceFromPlane( viewLightPos, frustum[0] ) < r ) &&
            ( GetSignedDistanceFromPlane( viewLightPos, frustum[1] ) < r ) &&
            ( GetSignedDistanceFromPlane( viewLightPos, frustum[2] ) < r ) &&
            ( GetSignedDistanceFromPlane( viewLightPos, frustum[3] ) < r ) )

            {
                Light1_4[lc] = i;
                lc++;
            }
    }
    
    Light1_4.a = lc; 
}

