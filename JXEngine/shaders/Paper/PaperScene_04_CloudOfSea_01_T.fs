#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 ScatterLight_Out;
layout(location = 2) out vec4 Transmittance_Out;
layout(location = 3) out vec4 FinalPos_Out;
in vec2 TexCoords;

uniform mat4 view;
uniform mat4 view_inv;
uniform mat4 view_pre;
uniform mat4 projection;
uniform mat4 projection_inv;
uniform mat4 projection_pre;
uniform int FrameIndex;

uniform sampler2D ScatterLight_In;
uniform sampler2D Transmittance_In;
uniform sampler2D FinalPos_In;

// TFRM
uniform float F_DIS;
uniform float F_ANG;
uniform float TEMPORAL_ACCELERATION_MUL;

#define AnimaControl 0.0


mat2 rot(in float a){float c = cos(a), s = sin(a);return mat2(c,s,-s,c);}
const mat3 m3 = mat3(0.33338, 0.56034, -0.71817, -0.87887, 0.32651, -0.15323, 0.15162, 0.69596, 0.61339)*1.93;
float mag2(vec2 p){return dot(p,p);}
float linstep(in float mn, in float mx, in float x){ return clamp((x - mn)/(mx - mn), 0., 1.); }
float prm1 = 0.;
vec2 bsMo = vec2(0);

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
	vec4 scatteredLight = pre_scat;
    float fogT = pre_scat.x;
    const float ldst = 8.;
	vec3 lpos = vec3(disp(time + ldst)*0.5, time + ldst);
	float t = 1.5;
    float stepsNum = MAX_STEP_NUM;

    float pre_t = length(pre_pos.xyz - ro);
    t = max(pre_t, t);

    stepsNum *= TEMPORAL_ACCELERATION_MUL;
    //stepsNum = t >= tmm.y ? 0 : stepsNum;

	for(int i=0; i<stepsNum; i++)
	{
		if(scatteredLight.a > 0.99)break;

		vec3 pos = ro + t * rd;
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
    pre_pos = vec4(ro + t * rd, pre_pos.w);
    pre_scat = clamp(scatteredLight, 0.0, 1.0);
    pre_transmittance = vec4(fogT, 0.0, 0.0, 1.0);
	return pre_scat;
}

bool HistoryRejection(in vec3 N1)
{
    float FOVEA_Factor = 1.0 - Map_HSV(HSV_PDF(degrees(acos(dot(N1, view_center_dir))))); /* 值越小，越触发 */
    const float Const_Factor = 0.05;
    //return F_ANG > 1.01;
    //return F_ANG + (FOVEA_Factor * Const_Factor)  > 1.04;
    // F_ANG / (FOVEA_Factor * Const_Factor) < 1.0;
    return F_DIS > 1.14 || F_ANG > 1.005;
}

void main()
{
    vec2 q = TexCoords;
    vec2 p = (TexCoords * Resolution - 0.5 * Resolution.xy) / Resolution.y;
    
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * (vec4(ReMapNDC_V3(vec3(TexCoords, 1.0)), 1.0) * far_plane);
    vec3 dir  = normalize(rd.xyz - orig);
    prm1 = smoothstep(-0.4, 0.4, sin(AnimaControl * 0.3));

    // Read history
    vec4 pos_tex = vec4(orig, 0.0);
    vec4 scat_tex = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 trans_tex = vec4(0.0, 1.0, 1.0, 1.0);
    
    // Read Pre Data
    vec3 M_pos = orig + dir;
    vec4 M_ndc = projection_pre * view_pre * vec4(M_pos, 1.0);
    M_ndc = M_ndc / M_ndc.w;
    vec2 M_uv = M_ndc.xy * 0.5 + 0.5;
    vec3 M_dir = normalize(M_pos - LastViewPosWS);

    // Solve History Data
    bool isFirstFrame = FrameIndex == 0;
    pos_tex = mix(texture(FinalPos_In, M_uv), vec4(orig, 0.0), isFirstFrame);
    scat_tex = mix(texture(ScatterLight_In, M_uv), vec4(0.0, 0.0, 0.0, 0.0), isFirstFrame);
    trans_tex = mix(texture(Transmittance_In, M_uv), vec4(0.0, 1.0, 1.0, 1.0), isFirstFrame);
    
    bool isOutRange = inRange(M_uv) < -0.05;
    pos_tex = mix(pos_tex, vec4(orig, 0.0), isOutRange);
    scat_tex = mix(scat_tex, vec4(0.0, 0.0, 0.0, 0.0), isOutRange);
    trans_tex = mix(trans_tex, vec4(0.0, 1.0, 1.0, 1.0), isOutRange);

    bool isReStart = HistoryRejection(dir);
    float blend_op = isReStart ? 0.6 : 0.0;
    pos_tex = mix(pos_tex, vec4(orig, 0.0), isReStart);
    scat_tex = mix(scat_tex, vec4(0.0, 0.0, 0.0, 0.0), blend_op);
    trans_tex = mix(trans_tex, vec4(0.0, 1.0, 1.0, 1.0), blend_op);

    bool isRefresh = FrameIndex % 200 == 0;
    pos_tex = mix(pos_tex, vec4(orig, 0.0), isRefresh);
    scat_tex = mix(scat_tex, vec4(0.0, 0.0, 0.0, 0.0), isRefresh);
    trans_tex = mix(trans_tex, vec4(0.0, 1.0, 1.0, 1.0), isRefresh);

	vec4 scn = render(orig, dir, AnimaControl * 3.0 , pos_tex, scat_tex, trans_tex);
	
    // Show
    vec3 col = scn.rgb;
    col = iLerp(col.bgr, col.rgb, clamp(1.-prm1,0.05,1.));
    col = pow(col, vec3(.55,0.65,0.6))*vec3(1.,.97,.9);
    col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.12)*0.7+0.3; //Vign
    FragColor = vec4(col.rgb, 1.0);
    ScatterLight_Out = scat_tex;
    Transmittance_Out = trans_tex;
    FinalPos_Out = pos_tex;
} 