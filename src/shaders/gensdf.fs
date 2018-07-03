#version 330

uniform vec3 dims;
uniform samplerBuffer points;
uniform samplerBuffer tids;
uniform usamplerBuffer indices;

uniform int numIndices;

uniform uint layer;

out vec4 outColor;

#define FLT_MAX 3.402823466e+38 
#define FLT_MIN -3.402823466e+37 

float dot2(vec3 v) { return dot(v,v); }
float hmax(in vec3 p) { return max(p.x,max(p.y,p.z));}
float hmin(in vec3 p) { return min(p.x,min(p.y,p.z));}

float box(int i, in vec3 from)
{
     vec3 d = abs(from - ivec3(texelFetch(points, i).xyz)) - vec3(1);
     return min(hmax(d), 0.0f) + length(max(d, vec3(0.0)));
}

// Moller-Trombore Algorithm
// Inefficient but works regardless of winding
int hit(const vec3 v0, const vec3 v1, const vec3 v2, const vec3 orig, const vec3 dir) 
{
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
    // Calculate planes normal vector
    vec3 pvec = cross(dir, e2);
    float det = dot(e1, pvec);

    // Ray is parallel to plane
    if (det < 1e-8 && det > -1e-8)
        return 0;

    float inv_det = 1 / det;
    vec3 tvec = orig - v0;
    float u = dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1)
        return 0;

    vec3 q = cross(tvec, e1);
    float v = dot(dir, q) * inv_det;
    if (v < 0 || u + v > 1)
        return 0;

    float t = inv_det * dot(e2, q);
    if (t < 0.0001)
        return 0;

    return 1; //dot(e2, qvec) * inv_det;
}

float tri(in vec3 v1, in vec3 v2, in vec3 v3, in vec3 from, out vec3 dir)
{
    vec3 v21 = v2 - v1; vec3 p1 = from - v1;
    vec3 v32 = v3 - v2; vec3 p2 = from - v2; 
    vec3 v13 = v1 - v3; vec3 p3 = from - v3;
    vec3 nor = cross( v21, v13 );

    dir = (p1+p2+p3)/3;

    float NdP1 = dot(nor,p1);

    return ( (sign(dot(cross(v21,nor),p1)) + 
              sign(dot(cross(v32,nor),p2)) + 
              sign(dot(cross(v13,nor),p3))  <2.0) 
           ?
              min( min( 
              dot2(v21*clamp(dot(v21,p1)/dot2(v21),0.0,1.0)-p1), 
              dot2(v32*clamp(dot(v32,p2)/dot2(v32),0.0,1.0)-p2) ), 
              dot2(v13*clamp(dot(v13,p3)/dot2(v13),0.0,1.0)-p3) )
           :
              NdP1*NdP1/dot2(nor) );
}

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;

    float closest = FLT_MAX;
    vec3 from = vec3(fragCoord, float(layer)); 
    vec3 dir;
    int hits = 0;
    for (int i = 0; i < numIndices; i+=3)
    {
	uint i1 = (texelFetch(indices, i+0).x);
	uint i2 = (texelFetch(indices, i+1).x);
	uint i3 = (texelFetch(indices, i+2).x);

	vec3 v1 = (texelFetch(points, int(i1)).xyz);
	vec3 v2 = (texelFetch(points, int(i2)).xyz);
	vec3 v3 = (texelFetch(points, int(i3)).xyz);

	float r = tri(v1, v2, v3, from, dir);
        hits += hit(v1, v2, v3, from, vec3(0,1,0));

        if (r < closest) closest = r;
    }

    float sn = (hits % 2 == 0) ? 1.0 : -1.0;

    outColor = vec4(sqrt(closest) * sn,1.0,1.0,1.0);
}
