#version 330

//external
SCREEN_WIDTH
SCREEN_HEIGHT
MAX_VIEWSPACE_MESHES
NUM_SAMPLES
TILE_SIZE
MAX_LIGHTS
//external end

uniform sampler3D modelSdf;

uniform int numBoundingBoxes;
layout(std140) uniform BoundingBoxes
{
    vec4 Bounds[MAX_VIEWSPACE_MESHES];
};
layout(std140) uniform Orientations
{
    mat4 Orients[MAX_VIEWSPACE_MESHES];
};

uniform uint layer;

uniform int gridSize = 512;

out vec4 outColor;

#define FLT_MAX 3.402823466e+38 
#define FLT_MIN -3.402823466e+37 

float dot2(const in vec3 v) { return dot(v,v); }
float hmax(const in vec3 p) { return max(p.x,max(p.y,p.z));}
float hmin(const in vec3 p) { return min(p.x,min(p.y,p.z));}
float hmax(const in vec2 p) { return max(p.x,p.y);}
float hmin(const in vec2 p) { return min(p.x,p.y);}
float insideBound(const in vec3 v, const in vec3 topRight) 
	{ return hmin( step(0, v) - step(topRight, v) ); }

float pack_ints(int a, int b) 
{
    return (a * 64.0) + b;
}

float scale = 2.56 * 512/gridSize;
float box(int i, const in vec3 from)
{
     mat4 orient = Orients[i];
     orient[3] /= scale; 
     vec3 f = (orient * vec4(from, 1.0)).xyz - ceil(Bounds[i].xyz / 2 / scale);
     vec3 d = (abs(f - vec3(gridSize/2)) - ceil(Bounds[i].xyz / 2 / scale));
     return (min(hmax(d), 0.0f) + length(max(d, vec3(0.0))));
}

float model(int i, in vec3 from )
{
    mat4 orient = Orients[i];
    orient[3] /= scale; 
    from = (orient * vec4(from, 1.0)).xyz; 
    from -= gridSize / 2;
    from *= scale;

    vec4 eps = vec4(-sign(from) / Bounds[i].xyz, 0.0);
    vec3 border = from / Bounds[i].xyz;

    if (insideBound(border, vec3(1.0)) == 1.0)
	return texture(modelSdf, border).r;

    border = clamp(border, 0.0, 1.0);
    float d = texture(modelSdf, border).r;
    vec3 dist_grad = vec3( texture(modelSdf, border + eps.xww).r - d, 
    			   texture(modelSdf, border + eps.wyw).r - d, 
    			   texture(modelSdf, border + eps.wwz).r - d ); 

    vec3 borderClosest = from - normalize(dist_grad) * d;

    return distance(borderClosest, from);
}

uniform int hittracking = 1;
void main()
{
    vec2 fragCoord = gl_FragCoord.xy;

    vec4 closest = vec4(FLT_MAX);
    vec2 index = vec2(30.0);
    vec3 from = vec3(fragCoord, float(layer)); 
    for (int i = 0; i < numBoundingBoxes; i++)
    {
	float r = model(i, from);
        if (r < closest.x) { closest.x = r; index.x = float(i); }
        else if (r < closest.y) { closest.y = r; index.y = float(i); }
    }

    if (hittracking == 1) {
	outColor = vec4(index.x, 30.0, 1.0, 1.0);
    } else {
    	outColor = vec4(closest.x,1.0,1.0,1.0);
    }
}
