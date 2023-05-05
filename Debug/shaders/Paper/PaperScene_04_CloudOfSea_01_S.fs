#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform mat4 view;
uniform mat4 view_inv;
uniform mat4 view_pre;
uniform mat4 projection;
uniform mat4 projection_inv;
uniform mat4 projection_pre;

#define AnimaControl 0.0

//StepAlloc Alg Select
#ifndef STEP_ALLOC_METHOD
    #define STEP_ALLOC_METHOD 1
#endif

#ifndef SHADOW_STEP_ALLOC_METHOD
    #define SHADOW_STEP_ALLOC_METHOD 1
#endif

mat2 rot(in float a){float c = cos(a), s = sin(a);return mat2(c,s,-s,c);}
const mat3 m3 = mat3(0.33338, 0.56034, -0.71817, -0.87887, 0.32651, -0.15323, 0.15162, 0.69596, 0.61339)*1.93;
float mag2(vec2 p){return dot(p,p);}
float linstep(in float mn, in float mx, in float x){ return clamp((x - mn)/(mx - mn), 0., 1.); }
float prm1 = 0.;
vec2 bsMo = vec2(0);
float eyeAngle = 1.0;

vec2 disp(float t)
{ 
    return vec2(0.0);
    return vec2(sin(t*0.22)*1., cos(t*0.175)*1.)*2.; 
}


float getsat(vec3 c)
{
    float mi = min(min(c.x, c.y), c.z);
    float ma = max(max(c.x, c.y), c.z);
    return (ma - mi)/(ma+ 1e-7);
}

//from https://www.shadertoy.com/view/lsdGzN
vec3 iLerp(in vec3 a, in vec3 b, in float x)
{
    vec3 ic = mix(a, b, x) + vec3(1e-6,0.,0.);
    float sd = abs(getsat(ic) - mix(getsat(a), getsat(b), x));
    vec3 dir = normalize(vec3(2.*ic.x - ic.y - ic.z, 2.*ic.y - ic.x - ic.z, 2.*ic.z - ic.y - ic.x));
    float lgt = dot(vec3(1.0), ic);
    float ff = dot(dir, normalize(ic));
    ic += 1.5*dir*sd*ff*lgt;
    return clamp(ic,0.,1.);
}

vec2 map(vec3 p)
{
    vec3 p2 = p;
    p2.xy -= disp(p.z).xy;
    p.xy *= rot(sin(p.z+AnimaControl)*(0.1 + prm1*0.05) + AnimaControl*0.09);
    float cl = mag2(p2.xy);
    float d = 0.;
    p *= .61;
    float z = 1.;
    float trk = 1.;
    float dspAmp = 0.1 + prm1*0.2;
    for(int i = 0; i < 5; i++)
    {
		p += sin(p.zxy*0.75*trk + AnimaControl *trk*.8)*dspAmp;
        d -= abs(dot(cos(p), sin(p.yzx))*z);
        z *= 0.57;
        trk *= 1.4;
        p = p*m3;
    }
    d = abs(d + prm1*3.)+ prm1*.3 - 2.5 + bsMo.y;
    return vec2(d + cl*.2 + 0.25, cl);
}

vec4 render( in vec3 ro, in vec3 rd, float time, inout vec4 pre_pos, inout vec4 pre_scat, inout vec4 pre_transmittance)
{
   // if(abs(p.x) - 0.001 < 0.0)return vec4(0.0, 0.0, 0.0, 1.0);
	vec4 scatteredLight = vec4(0);
    const float ldst = 8.;
	vec3 lpos = vec3(disp(time + ldst)*0.5, time + ldst);
	float t = 1.5;
	float fogT = 0.;

#if STEP_ALLOC_METHOD == 1
    bool isFoveal = true;
    float fovealIndensity = IsFovealRegion(eyeAngle, 1.0, 1.0, isFoveal);
    float stepsNum = ConstantStepAlloc(fovealIndensity, MIN_STEP_NUM, MAX_STEP_NUM);
    //float dt = .2 - .195*cos(iTime*.05);//animated
#elif STEP_ALLOC_METHOD == 2
    float stepsNum = FoveatedStepAlloc(eyeAngle, MIN_STEP_NUM, MAX_STEP_NUM);
#else
    float stepsNum = MAX_STEP_NUM;
#endif


	for(int i=0; i<stepsNum; i++)
	{
		if(scatteredLight.a > 0.99)break;

		vec3 pos = ro + t*rd;
        vec2 mpv = map(pos);
		float den = clamp(mpv.x-0.3,0.,1.)*1.12;
		float dn = clamp((mpv.x + 2.),0.,3.);
        
		vec4 col = vec4(0);
        if (mpv.x > 0.6)
        {
        
            col = vec4(sin(vec3(5.,0.4,0.2) + mpv.y*0.1 +sin(pos.z*0.4)*0.5 + 1.8)*0.5 + 0.5,0.08);
            col *= den*den*den;
			col.rgb *= linstep(4.,-2.5, mpv.x)*2.3;
            float dif =  clamp((den - map(pos+.8).x)/9., 0.001, 1. );
            dif += clamp((den - map(pos+.35).x)/2.5, 0.001, 1. );
            col.xyz *= den*(vec3(0.005,.045,.075) + 1.5*vec3(0.033,0.07,0.03)*dif);
        }
		
		float fogC = exp(t*0.2 - 2.2);
		col.rgba += vec4(0.06,0.11,0.11, 0.1)*clamp(fogC-fogT, 0., 1.);
		fogT = fogC;
		scatteredLight = scatteredLight + col*(1. - scatteredLight.a);
		t += clamp(0.5 - dn*dn*.05, 0.09, 0.3);
	}
	return clamp(scatteredLight, 0.0, 1.0);
}


void main()
{
    vec2 q = TexCoords;
    vec2 p = (TexCoords * Resolution - 0.5 * Resolution.xy) / Resolution.y;
    
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * (vec4(ReMapNDC_V3(vec3(TexCoords, 1.0)), 1.0) * far_plane);
    vec3 dir  = normalize(rd.xyz - orig);
    prm1 = smoothstep(-0.4, 0.4, sin(AnimaControl * 0.3));

    eyeAngle = degrees(acos(dot(dir, view_center_dir)));

    // Read history
    vec4 pos_tex;
    vec4 scat_tex;
    vec4 trans_tex;

     
    pos_tex = vec4(orig, 0.0);
    scat_tex = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 scn = render(orig, dir, AnimaControl * 3.0 , pos_tex, scat_tex, trans_tex);
	
    // Show
    vec3 col = scn.rgb;
    col = iLerp(col.bgr, col.rgb, clamp(1.-prm1,0.05,1.));
    col = pow(col, vec3(.55,0.65,0.6))*vec3(1.,.97,.9);
    col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.12)*0.7+0.3; //Vign
    FragColor = vec4(col.rgb, 1.0);
} 