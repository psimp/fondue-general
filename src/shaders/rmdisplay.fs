#version 330 core

#define SCREEN_WIDTH 1600.0f
#define SCREEN_HEIGHT 900.0f

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D image;
uniform int iFrame;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 iResolution = vec2(SCREEN_WIDTH, SCREEN_HEIGHT);    

    vec2 uv = fragCoord.xy / iResolution.xy;

    vec3 col = vec3(0.0);
    
    if( iFrame>0 )
    {
        col = texture( image, uv ).xyz;
        col /= float(iFrame);
        col = pow( col, vec3(0.4545) );
    }
    
    // color grading and vigneting
    col = pow( col, vec3(0.8,0.85,0.9) );
    
    col *= 0.5 + 0.5*pow( 16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y), 0.1 );
    
    FragColor = vec4( col, 1.0 );
}

