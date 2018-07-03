#version 330 core

#define SCREEN_WIDTH 1600.0f
#define SCREEN_HEIGHT 900.0f
#define FLT_MAX 3.402823466e+38 
#define FLT_MIN -3.402823466e+37 

uniform sampler3D sdf;
uniform usampler3D kernels;

uniform vec3 viewPos;
uniform mat4 viewMatrix;

uniform float cTime;

out vec4 outColor;

float hash(float seed)
{
    return fract(sin(seed)*43758.5453);
}

vec3 cosineDirection( in float seed, in vec3 nor )
{
    float u = hash( 78.233 + seed );
    float v = hash( 10.873 + seed );

    float a = 6.2831853 * v;
    u = 2.0 * u - 1.0;
    return normalize( nor + vec3(sqrt(1.0 - u*u) * vec2(cos(a), sin(a)), u) );
}

float maxcomp(in vec3 p) { return max(p.x, max(p.y, p.z)); }

vec3 calcNormal(in vec3 pos)
{
    vec3 eps = vec3(0.0001, 0.0, 0.0);

    return normalize( vec3(
	map( pos+eps.xyy ) - map ( pos-eps.xyy ),
	map( pos+eps.yxy ) - map ( pos-eps.yxy ),
	map( pos+eps.yyx ) - map ( pos-eps.yyx ) ) );
}

float intersectAABB(vec3 o, vec3 d) 
{
    float tmin = FLT_MIN, tmax = FLT_MAX, ct1 = 0.0f, ct2 = 0.0f;
 
    vec3 id = (1 / d) + vec3(0.000001f);

    ct1 = (0.0f - o.x) * id.x;
    ct2 = (256.0f - o.x) * id.x;

    tmin = max(tmin, min(ct1, ct2));
    tmax = min(tmax, max(ct1, ct2));
 
    ct1 = (0.0f - o.y) * id.y;
    ct2 = (256.0f - o.y) * id.y;
 
    tmin = max(tmin, min(ct1, ct2));
    tmax = min(tmax, max(ct1, ct2));

    ct1 = (0.0f - o.z) * id.z;
    ct2 = (256.0f - o.z) * id.z;
 
    tmin = max(tmin, min(ct1, ct2));
    tmax = min(tmax, max(ct1, ct2));
 
    if( tmax >= tmin && tmin >= 0.0f )
	return tmin;
    else 
	return FLT_MAX;
}

float sparse_trace(vec3 o, vec3 d, float t0)
{
    float nt = 0.0f;
    vec3 cr;

    for (uint i = 0u; i < 64u; i++)
    {
        cr = (o + t0 * d) / vec3(256);

	if ( cr.x > 0.999999f || cr.y > 0.999999f || cr.z > 0.999999f || cr.x < -0.000001f || cr.y < -0.000001f || cr.z < -0.000001f )
	{
	   return FLT_MAX; // Ray exited distance field
	}
	else
	{
           nt = texture(sdf, cr).r;
	}

        t0 += nt * 1.0f;
    }

    return t0;
}

float shadow( in vec3 o, in vec3 d, float mint, float maxt, float k)
{
    vec3 cr;
    float h = 0.0f, res = 1.0f;

    for( float t=mint; t < maxt; )
    {
	cr = (o + t * d) / vec3(256);

	if ( cr.x > 0.999999f || cr.y > 0.999999f || cr.z > 0.999999f || cr.x < -0.000001f || cr.y < -0.000001f || cr.z < -0.000001f )
	{
	   return 1.0f; // Ray exited distance field
	}
	else
	{
           h = texture(sdf, cr).r;
	}

        if( h<0.001f )
            return 0.0f;
	res = min(res, k*h/t);
        t += h;
    }
    return res;
}

vec3 calcLightIndirect(vec3 o, vec3 d, float seed)
{
    const float EPSILON = 0.0001f;

    vec3 colorMask = vec3(1.0f);
    vec3 colorAcc = vec3(0.0f);

    float fdis = 0.0f;
    for ( uint bounce = 0; bounce < 3; bounce++ )
    {
	float t = intersect ( o, d );
	if ( t < 0.0f )
	{
	    if ( bounce == 0 ) return mix(0.05 * vec3(0.9, 1.0, 1.0), skyCol, smoothstep(0.1, 0.25, rd.y) );
	    break;
	}
	if (bounce == 0 ) fdis = t;
	
	vec3 pos = o + d * t;
	vec3 normal = calcNormal( pos );
	vec3 surfaceColor = vec3(0.4f) * vec3(1.2f, 1.1f, 1.0f);

	colorMask *= surfaceColor;

	vec3 iColor = vec3(0.0f);

	// LIGHT 1
	float sunDif = max(0.0f, dot(sunDir, normal));
	float sunSha = (sunDif > 0.00001) shadow( pos + normal * EPSILON, sunDir) : 1.0f;
	iColor += sunCol * sunDif * sunSha;

	// LIGHT 2
	vec3 skyPoint = cosineDirection(seed + 7.1 * float(cFrame) + 5681.123 + float(bounce) * 92.13, normal);
	float skySha = shadow( pos + normal * EPSILON, skyPoint);
	iColor += skyCol * skySha;

	colorAcc += colorMask * iColor;

	d = cosineDirection(76.2 + 73.1*float(bounce) + seed + 17.7*float(cFrame), normal);
	o = pos;
    }

    float ff = exp(-0.01 * fdis * fdis);
    colorAcc *= ff;
    colorAcc += (1.0f - ff) * 0.05f * vec3(0.9f, 1.0f, 1.0f);

    return colorAcc;
}

void main() 
{
    vec3 uvw = vec3(gl_FragCoord.xy / vec2(SCREEN_WIDTH, SCREEN_HEIGHT), 1.0f) * 2 - 1;
    uvw.r *= SCREEN_WIDTH / SCREEN_HEIGHT;

    vec3 o = viewPos;
    vec3 d = transpose(mat3(viewMatrix)) * normalize(uvw);
    float t = intersectAABB(o, d);
   
    float sdw = 1.0f;
    if (t < FLT_MAX)
    {
        t = sparse_trace(o, d, t);
        sdw = shadow(o + d * t, normalize(vec3(1, 1, 1)), 1.5f, 200.0f, 1);  
    }

    vec3 color = vec3(1 / (1 + t * t * 0.00001));

    outColor = vec4( color * sdw, 1.0f);
}
