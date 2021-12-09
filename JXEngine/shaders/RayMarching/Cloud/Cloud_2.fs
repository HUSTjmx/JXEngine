#version 330 core
#define D_DEMO_FREE
//#define D_DEMO_SHOW_IMPROVEMENT_FLAT
//#define D_DEMO_SHOW_IMPROVEMENT_NOISE
//#define D_DEMO_SHOW_IMPROVEMENT_FLAT_NOVOLUMETRICSHADOW
//#define D_DEMO_SHOW_IMPROVEMENT_NOISE_NOVOLUMETRICSHADOW
#ifdef D_DEMO_FREE
	// Apply noise on top of the height fog?
    #define D_FOG_NOISE 1.0

	// Height fog multiplier to show off improvement with new integration formula
    #define D_STRONG_FOG 0.0

    // Enable/disable volumetric shadow (single scattering shadow)
    #define D_VOLUME_SHADOW_ENABLE 0

	// Use imporved scattering?
	// In this mode it is full screen and can be toggle on/off.
	#define D_USE_IMPROVE_INTEGRATION 1
//
// Pre defined setup to show benefit of the new integration. Use D_DEMO_FREE to play with parameters
//
#elif defined(D_DEMO_SHOW_IMPROVEMENT_FLAT)
    #define D_STRONG_FOG 10.0
    #define D_FOG_NOISE 0.0
	#define D_VOLUME_SHADOW_ENABLE 1
#elif defined(D_DEMO_SHOW_IMPROVEMENT_NOISE)
    #define D_STRONG_FOG 5.0
    #define D_FOG_NOISE 1.0
	#define D_VOLUME_SHADOW_ENABLE 1
#elif defined(D_DEMO_SHOW_IMPROVEMENT_FLAT_NOVOLUMETRICSHADOW)
    #define D_STRONG_FOG 10.0
    #define D_FOG_NOISE 0.0
	#define D_VOLUME_SHADOW_ENABLE 0
#elif defined(D_DEMO_SHOW_IMPROVEMENT_NOISE_NOVOLUMETRICSHADOW)
    #define D_STRONG_FOG 3.0
    #define D_FOG_NOISE 1.0
	#define D_VOLUME_SHADOW_ENABLE 0
#endif
// Used to control wether transmittance is updated before or after scattering (when not using improved integration)
// If 0 strongly scattering participating media will not be energy conservative
// If 1 participating media will look too dark especially for strong extinction (as compared to what it should be)
// Toggle only visible zhen not using the improved scattering integration.
#define D_UPDATE_TRANS_FIRST 0

// Apply bump mapping on walls
#define D_DETAILED_WALLS 0

// Use to restrict ray marching length. Needed for volumetric evaluation.
#define D_MAX_STEP_LENGTH_ENABLE 1

// Light position and color
#define LPOS vec3( 20.0+15.0*sin(iTime*0.1), 15.0+18.0*cos(iTime*0.1),-20.0)
#define LCOL (600.0 * vec3( 1.0, 0.9, 0.5))

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 LastColor;
layout(location = 2) out vec4 ScatterLight;
layout(location = 3) out vec4 FinalPos;

in vec2 TexCoords;

uniform sampler2D noisyTexture;
uniform sampler2D preTexture;
uniform sampler2D scatterTex;
uniform sampler2D posTex;
uniform mat4 view;
uniform mat4 view_inv;
uniform mat4 view_pre;
uniform mat4 projection;
uniform mat4 projection_inv;
uniform mat4 projection_pre;

float displacementSimple( vec2 p )
{
    float f;
    p = p * vec2(0.2);
    f  = 0.5000* texture( noisyTexture, p).x; p = p*2.0;
    f += 0.2500* texture( noisyTexture, p).x; p = p*2.0;
    f += 0.1250* texture( noisyTexture, p).x; p = p*2.0;
    f += 0.0625* texture( noisyTexture, p).x; p = p*2.0;
    return f;
}

vec3 getSceneColor(vec3 p, float material)
{
	if(material == 1.0)
	{
		return vec3(1.0, 0.5, 0.5);
	}
	else if(material == 2.0)
	{
		return vec3(0.5, 1.0, 0.5);
	}
	else if(material == 3.0)
	{
		return vec3(0.5, 0.5, 1.0);
	}
	return vec3(0.0, 0.0, 0.0);
}

float getClosestDistance(vec3 p, out float material)
{
	float d = 0.0;
#if D_MAX_STEP_LENGTH_ENABLE
    float minD = 1.0; // restrict max step for better scattering evaluation
#else
	float minD = 10000000.0;
#endif
	material = 0.0;
    
    float yNoise = 0.0;
    float xNoise = 0.0;
    float zNoise = 0.0;
#if D_DETAILED_WALLS
    yNoise = 1.0*clamp(displacementSimple(p.xz*0.005),0.0,1.0);
    xNoise = 2.0*clamp(displacementSimple(p.zy*0.005),0.0,1.0);
    zNoise = 0.5*clamp(displacementSimple(p.xy*0.01),0.0,1.0);
#endif
    
	d = max(0.0, p.y - yNoise);
	if(d<minD)
	{
		minD = d;
		material = 2.0;
	}
	
	d = max(0.0,p.x - xNoise);
	if(d<minD)
	{
		minD = d;
		material = 1.0;
	}
	
	d = max(0.0,40.0-p.x - xNoise);
	if(d<minD)
	{
		minD = d;
		material = 1.0;
	}
	
	d = max(0.0,-p.z - zNoise);
	if(d<minD)
	{
		minD = d;
		material = 3.0;
    }
    
	return minD;
}

vec3 calcNormal( in vec3 pos)
{
    float material = 0.0;
    vec3 eps = vec3(0.3,0.0,0.0);
	return normalize( vec3(
           getClosestDistance(pos+eps.xyy, material) - getClosestDistance(pos-eps.xyy, material),
           getClosestDistance(pos+eps.yxy, material) - getClosestDistance(pos-eps.yxy, material),
           getClosestDistance(pos+eps.yyx, material) - getClosestDistance(pos-eps.yyx, material) ) );
}

vec3 evaluateLight(in vec3 pos)
{
    vec3 lightPos = LPOS;
    vec3 lightCol = LCOL;
    vec3 L = lightPos-pos;
    return lightCol * 1.0/dot(L,L);
}

vec3 evaluateLight(in vec3 pos, in vec3 normal)
{
    vec3 lightPos = LPOS;
    vec3 L = lightPos-pos;
    float distanceToL = length(L);
    vec3 Lnorm = L/distanceToL;
    return max(0.0,dot(normal,Lnorm)) * evaluateLight(pos);
}

// To simplify: wavelength independent scattering and extinction
void getParticipatingMedia(out float sigmaS, out float sigmaE, in vec3 pos)
{
    float heightFog = 7.0 + D_FOG_NOISE*3.0*clamp(displacementSimple(pos.xz*0.005 + iTime*0.01),0.0,1.0);
    heightFog = 0.3*clamp((heightFog-pos.y)*1.0, 0.0, 1.0);
    
    const float fogFactor = 1.0 + D_STRONG_FOG * 5.0;
    
    const float sphereRadius = 5.0;
    float sphereFog = clamp((sphereRadius - length(pos - vec3(20.0, 19.0, -17.0)))/sphereRadius, 0.0,1.0);
    
    const float constantFog = 0.02;

    sigmaS = constantFog + heightFog * fogFactor + sphereFog;
   
    const float sigmaA = 0.0;
    sigmaE = max(0.000000001, sigmaA + sigmaS); // to avoid division by zero extinction
}

float phaseFunction()
{
    return 1.0 / (4.0 * 3.14);
}

float volumetricShadow(in vec3 from, in vec3 to)
{
#if D_VOLUME_SHADOW_ENABLE
    const float numStep = 16.0; // quality control. Bump to avoid shadow alisaing
    float shadow = 1.0;
    float sigmaS = 0.0;
    float sigmaE = 0.0;
    float dd = length(to-from) / numStep;
    for(float s = 0.5; s < (numStep-0.1); s += 1.0)// start at 0.5 to sample at center of integral part
    {
        vec3 pos = from + (to-from)*(s/(numStep));
        getParticipatingMedia(sigmaS, sigmaE, pos);
        shadow *= exp(-sigmaE * dd);
    }
    return shadow;
#else
    return 1.0;
#endif
}

void traceScene(bool improvedScattering, vec3 rO, vec3 rD, inout vec3 finalPos, inout vec3 normal, inout vec3 albedo, inout vec4 scatTrans)
{
	const int numIter = 100;
	
    float sigmaS = 0.0;
    float sigmaE = 0.0;
    
    vec3 lightPos = LPOS;
    
    // Initialise volumetric scattering integration (to view)
    float transmittance = 1.0;
    vec3 scatteredLight = vec3(0.0, 0.0, 0.0);
    
	float d = 1.0; // hack: always have a first step of 1 unit to go further
	float material = 0.0;
	vec3 p = vec3(0.0, 0.0, 0.0);
    float dd = 0.0;
	for(int i = 0; i < numIter; ++i)
	{
        if(d > 100.0)break;
		vec3 p = rO + d*rD;
         
    	getParticipatingMedia(sigmaS, sigmaE, p);
        
#ifdef D_DEMO_FREE
        if(D_USE_IMPROVE_INTEGRATION>0) // freedom/tweakable version
#else
        if(improvedScattering)
#endif
        {
            // See slide 28 at http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/
            vec3 S = evaluateLight(p) * sigmaS * phaseFunction()* volumetricShadow(p,lightPos);// incoming light
            vec3 Sint = (S - S * exp(-sigmaE * dd)) / sigmaE; // integrate along the current step segment
            scatteredLight += transmittance * Sint; // accumulate and also take into account the transmittance from previous steps

            // Evaluate transmittance to view independentely
            transmittance *= exp(-sigmaE * dd);
        }
		else
        {
            // Basic scatering/transmittance integration
        #if D_UPDATE_TRANS_FIRST
            transmittance *= exp(-sigmaE * dd);
        #endif
            scatteredLight += sigmaS * evaluateLight(p) * phaseFunction() * volumetricShadow(p,lightPos) * transmittance * dd;
        #if !D_UPDATE_TRANS_FIRST
            transmittance *= exp(-sigmaE * dd);
        #endif
        }
        dd = getClosestDistance(p, material);
        if(dd < 0.2)
            break; // give back a lot of performance without too much visual loss
		d += dd;
	}
	
	albedo = getSceneColor(p, material);
	
    finalPos = rO + d * rD;
    
    normal = calcNormal(finalPos);
    
    scatTrans = vec4(scatteredLight, transmittance);
}

void main()
{
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * vec4(vec3(ReMapNDC_V2(TexCoords), 1.0), 1.0);
    vec3 dir = normalize(rd.xyz);

    vec3 finalPos = orig;
	vec3 albedo = vec3( 0.0, 0.0, 0.0 );
	vec3 normal = vec3( 0.0, 0.0, 0.0 );
    vec4 scatTrans = vec4( 0.0, 0.0, 0.0, 0.0 );

    traceScene( TexCoords.x > 0.5, orig, dir, finalPos, normal, albedo, scatTrans);

    //lighting
    vec3 color = (albedo/3.14) * evaluateLight(finalPos, normal) * volumetricShadow(finalPos, LPOS);
    // Apply scattering/transmittance
    color = color * scatTrans.w + scatTrans.xyz;
    
   
#ifndef D_DEMO_FREE
    // Separation line
    if(abs(fragCoord.x-(iResolution.x*0.5))<0.6)
        color.r = 0.5;
#endif

    vec4 pre_pos = projection_pre * view_pre * rd;
    pre_pos = pre_pos / pre_pos.w;
    vec2 uv = pre_pos.xy * 0.5 + 0.5;
    vec3 preCol = texture(preTexture, uv).xyz;

    float a = 1.0;
    if(inRange(uv, 0.01, 0.99) < 0.0000001) a = 1.0;

    color = (1.0 - a) * preCol + a * color;

    LastColor = vec4(color, 1.0);
    FinalPos = vec4(finalPos, 0.0);
    ScatterLight = scatTrans;

    color = pow(color, vec3(1.0/2.2)); // simple linear to gamma, exposure of 1.0
   // FragColor = vec4(uv.xxy ,1.0);
   // FragColor = vec4(TexCoords.xxy, 1.0);
    vec3 pos_tex = texture(posTex, uv).xyz;
    vec4 scatter_tex = texture(scatterTex, uv);
    FragColor = vec4(color, 1.0);
} 