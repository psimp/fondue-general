#version 330

//external
SCREEN_WIDTH
SCREEN_HEIGHT
MAX_VIEWSPACE_MESHES
NUM_SAMPLES
MAX_LIGHTS
//external end

#define FLT_MAX 3.402823466e+38 
#define FLT_MIN -3.402823466e+37 

#define when_eq(x, y)  (1.0 - abs(sign(x - y)))
#define when_neq(x, y) (abs(sign(x - y)))      
#define when_gt(x, y)  (max(sign(x - y), 0.0)) 
#define when_lt(x, y)  (max(sign(y - x), 0.0)) 
#define when_ge(x, y)  (1.0 - when_lt(x, y))   
#define when_le(x, y)  (1.0 - when_gt(x, y))   

#define and(a, b)  (a * b)            
#define or(a, b)   (min(a + b, 1.0))  
#define xor(a, b)  (mod((a + b), 2.0))
#define not(a)     (1.0 - a)        

out vec4 outColor;

uniform int numBoundingBoxes;
layout(std140) uniform BoundingBoxes
{
    vec4 Bounds[MAX_VIEWSPACE_MESHES];
};
layout(std140) uniform Orientations
{
    mat4 Orients[MAX_VIEWSPACE_MESHES];
};
layout(std140) uniform SampleHemisphere
{
    vec2 Samples[NUM_SAMPLES];
};

uniform sampler2D noisemap;

uniform sampler3D sdf;
uniform sampler3D sdfScene;
uniform sampler3D sdfSceneHits;

uniform int sceneGridSize = 512;
uniform int iFrame;

uniform mat4 viewMatrix;
uniform vec3 viewPos;

const vec3 sunDir 	  = normalize(vec3(0.0,1.0,0.0));
const vec3 sunCol 	  =  6.0*vec3(1.0,0.8,0.6);
const vec3 skyCol 	  =  4.0*vec3(0.2,0.35,0.5);
const vec3 surfaceColor   = vec3(0.4)*vec3(1.2,1.1,1.0);

float hmax(const in vec3 p) { return max(p.x,max(p.y,p.z)); }
float hmin(const in vec3 p) { return min(p.x,min(p.y,p.z)); }
float hmax(const in vec2 p) { return max(p.x,p.y); }
float hmin(const in vec2 p) { return min(p.x,p.y); }

float insideBound(const in vec3 v, const in vec3 topRight) { return hmin( step(0, v) - step(topRight, v) ); }
float insideBound(const in vec2 v, const in vec2 topRight) { return hmin( step(0, v) - step(topRight, v) ); }

vec3 cosineDirection( const in vec3 nor, int index)
{
    vec2 uv = Samples[index];
    
    float a = 6.2831853 * uv.y;
    uv.x = 2.0*uv.x - 1.0;
    return normalize( nor + vec3(sqrt(1.0-uv.x*uv.x) * vec2(cos(a), sin(a)), uv.x) );
}

float map( in vec3 cr, float exitStep, const in vec3 bbox_inv )
{
    cr *= bbox_inv;
    return (insideBound(cr, vec3(1.0)) == 0.0) ? exitStep : texture(sdf, cr).r;
}

float map( in vec3 cr, const in vec3 bbox_inv )
{
    return texture(sdf, cr * bbox_inv).r;
}

vec3 calcModelNormal( in vec3 cr, const in vec3 bbox_inv )
{
    vec4 eps = vec4(bbox_inv, 0.0);
    cr *= bbox_inv;

    float phi = texture( sdf, cr ).r;
    return normalize( vec3(
      texture( sdf, cr+eps.xww ).r - phi,
      texture( sdf, cr+eps.wyw ).r - phi,
      texture( sdf, cr+eps.wwz ).r - phi ) );
}

vec3 calcSceneNormal( in vec3 cr )
{
    vec3 eps = vec3(1.0/float(sceneGridSize), 0.0, 0.0);

    float phi = texture( sdfScene, cr ).r;
    return normalize( vec3(
      texture( sdfScene, cr+eps.xyy ).r - phi,
      texture( sdfScene, cr+eps.yxy ).r - phi,
      texture( sdfScene, cr+eps.yyx ).r - phi ) );
}

vec3 calcNormal_deriv( in vec3 pos)
{
    return normalize( cross( dFdy(pos), dFdx(pos) ) );
}

const float scale = 2.56;
float scene_shadow(const in vec3 ro, const in vec3 rd, in vec2 ignore, float st, float k)
{
    float h = 0.0; float res = 1.0;

    vec3 lro = ro/scale; vec3 lrd=rd/scale;
    for(int i=0; i<35; i++ )
    {
        vec3 cr = (lro+st*lrd)/sceneGridSize + 0.5;	
	if(insideBound(cr, vec3(1.0)) == 0.0) return res;
	h = texture(sdfScene, cr).r;
        if( h<0.0001 ) {
	    if( texture(sdfSceneHits, cr).rg == ignore) { return 0.0; }
	    else { return 0.0; }
	}
	res = min( res,k*h/st );
	st += h;
    }

    return res;
}

float intersectModel(const in vec3 ro, const in vec3 rd, 
	   	     const in vec3 bbox_inv, float t)
{
    float h = 10.0; int nclose = 0;
    for(int i=0; i<10; i++ )
    {
        h = map(ro+rd*t, 20.5f, bbox_inv);
	if( h < 2.5 ) nclose++;
	if( nclose >= 3 ) return t;
        if( abs(h)<0.01 ) return t;
        t += h;
    }
    
    return -1.0;
}

float intersectScene(const in vec3 ro, const in vec3 rd, out vec3 bbox_inv, out vec3 mro, 
		     out vec3 mrd, out int modelIndex, inout vec2 ignore, float st)
{
    float h = 0.0;

    vec3 lro = ro/scale; vec3 lrd=rd/scale; int nclose = 0;
    for(int i=0; i<30; i++ )
    {
        vec3 cr = (lro+st*lrd)/(sceneGridSize) + 0.5;	
	if(insideBound(cr, vec3(1.0)) == 0.0) return -1.0;
	h = texture(sdfScene, cr).r;
	if( h < 4.0 ) nclose++;
        if( h<0.0001 || nclose >= 10 )
	{
	    vec2 hits = texture(sdfSceneHits, cr).rg;
            ignore = hits;
	    
	    modelIndex = int(hits.x);

            mro = (Orients[modelIndex] * vec4(ro,1.0f)).xyz;
            mrd = mat3(Orients[modelIndex]) * rd;
    	    bbox_inv = 1 / Bounds[modelIndex].xyz;

	    return intersectModel(mro, mrd, bbox_inv, st); 
	}

	else st += h;
    }

    return -1.0;
}

float shadow_soft( const in vec3 roScene, const in vec3 rdScene, 
	      in vec3 roModel, in vec3 rdModel, 
	      in vec3 bbox_inv, in vec2 ignore, float ko, float ks )
{
    float tmax = 400.0; float res = 1.0;
    
    // Self-Shadowing
    float tself = 0.01;
    for( int i=0; i<30; i++ )
    {
        float h = map(roModel+rdModel*tself, 50.0f, bbox_inv);
	if( h>=50.0) break;
	if( h<0.001) break;
	res = min( res, ks*h/tself );
        tself += h;
    }
 
    return smoothstep(res, 0.5, 0.4);
}


float shadow( const in vec3 roScene, const in vec3 rdScene, 
	      in vec3 roModel, in vec3 rdModel, 
	      in vec3 bbox_inv, in vec2 ignore, float ko, float ks )
{
    float tmax = 400.0; float res = 1.0;
    
    // Self-Shadowing
    float tself = 0.01;
    for( int i=0; i<30; i++ )
    {
        float h = map(roModel+rdModel*tself, 50.0f, bbox_inv);
	if( h>=50.0) break;
	if( h<0.001) { res = 0.0001; break; }
	res = min( res, ks*h/tself );
        tself += h;
    }
 
    // Other-Shadowing
    float st = 1.9; 
    float res2 = scene_shadow(roScene, rdScene, ignore, st, ko);
   
    return min(res, res2);
}

vec3 calcRayCol(const in vec3 scenePos, const in vec3 sceneNor, const in vec3 modelPos, 
		const in vec3 modelNor, const in vec3 sceneLight, const in vec3 modelLight, 
		const in vec3 bbox_inv, const in vec2 ignore)
{
   const float shadowBias = 1.0;

   float sunDif = max(0.0, dot(modelLight, modelNor));
   float sunSha = 1.0; 
   if( sunDif > 0.00001 ) sunSha = 
	shadow(scenePos+20*shadowBias*sceneLight, sceneLight, modelPos+2*shadowBias*modelLight, 
	       modelLight, bbox_inv, ignore, 0.55, 0.1);

   float skySha = 1.0f;
   // Multple samples may provide better results at a major performance cost...
   for (int sDir = 0; sDir < 1; sDir++) {
        const vec3 env = vec3(0,1,0);

	skySha = shadow_soft(scenePos+20*shadowBias*env, env, modelPos+shadowBias*-env, 
	                  -env, bbox_inv, ignore, 0.9, 0.01);

   }

   return (skyCol * skySha * 0.07) + (4 * sunDif * sunSha * sunCol);
}

vec3 castRay( const in vec3 ro, const in vec3 rd )
{
    vec3 colorMask = vec3(1.0);
    vec3 cAcc = vec3(0.0);

    float t = -1.0; vec2 ignoreBox = vec2(30.0); float st = 0.001; int modelIndex;
    vec3 bbox_inv = vec3(-1.0); vec3 roModel; vec3 rdModel;
    t = intersectScene( ro, rd, bbox_inv, roModel, rdModel, modelIndex, ignoreBox, st);

    if ( t < 0.0 ) return mix( 0.05*vec3(0.9,1.0,1.0), skyCol, smoothstep(0.1,0.25,rd.y) );

    float fdis = t;

    vec3 modelPos = roModel + rdModel * t;
    vec3 scenePos = ro + rd * t;
    vec3 modelNor = calcModelNormal( modelPos, bbox_inv );
    vec3 sceneNor = calcSceneNormal( scenePos / (scale * sceneGridSize) + 0.5 );

    // #TODO Model light directions can be pre-calculated
    vec3 modelSun = normalize(mat3(Orients[modelIndex]) * sunDir);

    colorMask *= surfaceColor;

    cAcc += calcRayCol(scenePos, sceneNor, modelPos, modelNor, sunDir, 
		       modelSun, bbox_inv, ignoreBox) * colorMask;
    return cAcc;

    roModel = modelPos;

    // GI Rays:
//    colorMask *= surfaceColor;
//    vec3 bcAcc = vec3(0);
//    for (int iDir = 0; iDir < NUM_SAMPLES; iDir++)
//    {
//	vec3 brd = cosineDirection(1, nor, iDir);
//        float bt = softIntersectInSDF( roModel, brd, bbox_inv );
//	if (bt < 0.0001) break;
//    	vec3 bpos = roModel + brd * bt;
//	vec3 bnor = calcNormal(bpos, bbox_inv );
//        bcAcc += calcRayCol(bpos, bnor, bbox_inv );
//    }
//    bcAcc = (bcAcc * colorMask) / NUM_SAMPLES;
//    cAcc += bcAcc;

    float ff = exp(-0.00001*fdis*fdis);
    cAcc *= ff; 
    cAcc += (1.0-ff)*0.05*vec3(0.9,1.0,1.0);
    return cAcc;
}

const vec2 iResolution = vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
void main()
{
    vec3 uvw = ( vec3( (gl_FragCoord.xy - 0.5) / iResolution, 1.0f) * 2 - 1); 
    uvw.r *= (SCREEN_WIDTH / SCREEN_HEIGHT);

    outColor = vec4( pow( pow( castRay(viewPos, normalize(uvw) * mat3(viewMatrix)), vec3(0.4545) )
		          , vec3(0.8, 0.85, 0.9) ), 1.0);

}

#if 0
// Dynamic intersections (slow)

void intersectScene_dynamic()
{
    vec3 BoundingBox;
    HitRecord hlist[5];
    float ctmin = FLT_MAX; float ctmax = 0; int dl = 0; int dr = 0;
    for (int i = 0; i < numBoundingBoxes; i++) {
	vec3 rro = ro; vec3 rrd = rd; vec3 bbox;
	float nt = intersectAABB(rro, rrd, i, bbox) + 0.1;
	if (nt == -1.0) continue;

	int o = 0;
	if (nt < ctmin) { o = max(dl--,-2); ctmin = nt; }
	if (nt > ctmax) { o = min(dr++, 2); ctmax = nt; }
        hlist[o+2] = HitRecord(nt, rro, rrd, bbox);
    }
}

float intersectAABB(inout vec3 o, inout vec3 d, int index, out vec3 BoundingBox_inv) 
{
    mat4 orient = Orients[index];
    vec3 bound = Bounds[index].xyz;
    
    vec3 roModel = (orient * vec4(o,1.0f)) .xyz;
    vec3 rdModel = (orient * vec4(d,0.0f)) .xyz;

    if( cless(roModel, bound) && (hmin(roModel) >= 0) ) {
	o = roModel;
	d = rdModel;
	BoundingBox_inv = 1 / bound;
	return 0.0;
    }

    vec3 id = 1 / (rdModel + vec3(0.000001f));

    vec3 t1 = (      - roModel) * id;
    vec3 t2 = (bound - roModel) * id;

    float tmin = hmax( min(t1, t2) );
    float tmax = hmin( max(t1, t2) );

    if( tmax >= tmin && tmin >= 0.0f ) {
	o = roModel;
	d = rdModel;
	BoundingBox_inv = 1 / bound;
	return tmin;
    }
    else return -1.0;
}
#endif

