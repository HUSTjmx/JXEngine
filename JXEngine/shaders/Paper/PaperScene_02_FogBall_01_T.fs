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

// Fog Material Settings
#define SCATTERING  (0.7 * vec3(0.95, 0.5, 0.0))
#define ABSORPTION  (0.0 * vec3(0.75, 0.5, 0.0))
//0, 1 or 2
#define BASIC_ANIMATED_MEDIA 0
// 0, 1
#define ROTATE_MEDIA 0

// Light
#define LIGHT_DENSITY 100.0
#define LIGHT_COLOR (vec3(1.0, 1.0, 1.0) * LIGHT_DENSITY)
#define LIGHT_SIZE 0.3

// Box
#define BOX_SIZE vec3(8.0, 8.0, 8.0)
// White Wall
#define OBJ_WHITEWALL 1.0
    const vec3 WhiteWallColor = vec3(0.7295, 0.7355, 0.729);
// Right Wall
#define OBJ_RIGHTWALL 2.0
    const vec3 RightWallColor = vec3(0.117, 0.4125, 0.115);
// Left Wall
#define OBJ_LEFTWALL 3.0
    const vec3 LeftWallColor = vec3(0.611, 0.0555, 0.062);
// YuShi Sphere
#define OBJ_YUSHI_SPHERE 4.0
// 0 : (default) sphere ; 1 : Torus ; 2 : Octahedron
#define OBJ_TYPE 1
    const vec3 YuShiPos = vec3(0.0, 0.0, -1.5);
#if OBJ_TYPE == 0
    const float YushiRadius = 2.0;
#elif OBJ_TYPE == 1
    const vec2 YushiRadius = vec2(1.0, 0.5);
#elif OBJ_TYPE == 2
    const float YushiRadius = 1.2;
#elif OBJ_TYPE == 3
    const float YushiRadius = 2.2;
#endif
    const float rotateSpeed = 0.3;

// Wall Material
#define WALL_METALLIC 0.0
#define WALL_REFLECTANCE 0.1
#define WALL_ROUGHNESS 0.9

// Shade Settings
#define ShadowIntensity 1.0

// Phase Func Mode
#define PHASE_MODE 2.0
#define PHASE_G 0.5

float rand(vec3 co)
{
    return -1.0 + fract(sin(dot(co.xy,vec2(12.9898 + co.z,78.233))) * 43758.5453) * 2.0;
}

/*
float HSV_PDF(float e)
{
    const float M = 1.0;
    if(e <= 5.79)return 1.0 * M * 1.0;
    return 7.49 / pow(0.3 * e + 1.0, 2.0) * M;
}

float Map_HSV(float v)
{
    return exp(1.0 - v) - 1.0;
}

vec3 StretchToFarPLane(in vec3 pos)
{
    vec3 v = normalize(pos - LastViewPosWS);
    float cos_theta = dot(normalize(Last_view_center_dir), v);
    float len = far_plane / cos_theta;
    //return vec3(far_plane / len);
    return LastViewPosWS + v * vec3(len);
}
*/


float PhaseFunc(vec3 V, vec3 L, float g)
{
    float mu = dot(V, L);
    if(PHASE_MODE < 0.5) return PhaseFunc_CON();
    if(PHASE_MODE < 1.5) return PhaseFunc_RAYLEIGH(mu);
    if(PHASE_MODE < 2.5) return PhaseFunc_MIE(mu, g);
    return PhaseFunc_CON();
}

#if OBJ_TYPE == 0
float sdPlane(vec3 p, vec3 n /* normalized */, float h)
{
    return dot(p, n) + h;
}

float sdSphere(vec3 p, vec3 center, float s)
{
    return length(p - center) - s;
}
#endif

#if OBJ_TYPE == 1
float sdPlane(vec3 p, vec3 n /* normalized */, float h)
{
    return dot(p, n) + h;
}

float sdSphere(vec3 p, vec3 center, float s)
{
    return length(p - center) - s;
}
float sdTorus(vec3 p, vec3 center, vec2 t)
{
    p = p - center;
    vec2 q = vec2(length(p.xy) - t.x, p.z);
    return length(q) - t.y;
}
#endif

#if OBJ_TYPE == 2
float sdPlane(vec3 p, vec3 n /* normalized */, float h)
{
    return dot(p, n) + h;
}

float sdSphere(vec3 p, vec3 center, float s)
{
    return length(p - center) - s;
}
float sdOctahedron( vec3 p, vec3 center, float s)
{
    p = p - center;
    p = abs(p);
    return (p.x+p.y+p.z-s)*0.57735027;
}
#endif

#if OBJ_TYPE == 3

#define M_PI (3.14159)
#define M_PI_2 (2.0*M_PI)
#define M_PI_O_2 (M_PI/2.0)

vec2 rotate(vec2 v, float angle){
    float c = cos(angle);
    float s = sin(angle);
    return vec2(
        c * v.x - s * v.y,
        s * v.x + c * v.y
    );
}

// vector to transform, translation to apply, ZYX euler angles *in degrees*
vec3 transform(vec3 p, vec3 t, vec3 r){
    vec3 rP = p - t;
    vec3 angles = M_PI/180.0 * r;
    rP.xy = rotate(rP.xy, angles.x);
    rP.xz = rotate(rP.xz, angles.y);
    rP.yz = rotate(rP.yz, angles.z);
    return rP;
}


float sdPlane(vec3 p, vec3 n /* normalized */, float h)
{
    return dot(p, n) + h;
}

float sdSphere(vec3 p, vec3 center, float s)
{
    return length(p - center) - s;
}

float sdBox(vec3 p, vec3 r){
    vec3 d = abs(p) - (r);
    return length(max(d, 0.0)) + min(max(d.x,max(d.y,d.z)),0.0);
}

float sdSmoothBox(vec3 p, vec3 r, float s){
    vec3 d = abs(p) - (r - s);
    return length(max(d, 0.0)) - s + min(max(d.x,max(d.y,d.z)),0.0);
}

float sdPlaneH(vec3 p, float h){
    float dist = p.y + h;
    return max(abs(dist), 0.0);
}

float sdVerticalCapsule(vec3 p, float h, float r){
    p.y -= clamp( p.y, 0.0, h );
    return length( p ) - r;
}

float sdCone(vec3 p, float h, float r1){
    vec2 q = vec2( length(p.xz), p.y );
    
    vec2 k1 = vec2(0.0,h);
    vec2 k2 = vec2(-r1,2.0*h);
    vec2 ca = vec2(q.x-min(q.x,(q.y < 0.0)?r1:0.0), abs(q.y)-h);
    vec2 cb = q - k1 + k2*clamp( dot(k1-q,k2)/dot(k2,k2), 0.0, 1.0 );
    float s = (cb.x < 0.0 && ca.y < 0.0) ? -1.0 : 1.0;
    return s*sqrt( min(dot(ca,ca),dot(cb,cb)) );
}

float sdRoundCone(vec3 p, float r1, float r2, float h){
    vec2 q = vec2( length(p.xz), p.y );
    
    float b = (r1-r2)/h;
    float a = sqrt(1.0-b*b);
    float k = dot(q,vec2(-b,a));
    
    if( k < 0.0 ) return length(q) - r1;
    if( k > a*h ) return length(q-vec2(0.0,h)) - r2;
        
    return dot(q, vec2(a,b) ) - r1;
}

float sdCylinder(vec3 p, vec2 h){
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

/* pos, main radius/height, cap radius,  */
float sdRoundedCylinder(vec3 p,  vec2 h, float rb){
    vec2 d = vec2( length(p.xz)-2.0*h.x+rb, abs(p.y) - h.y );
    return min(max(d.x,d.y),0.0) + length(max(d,0.0)) - rb;
}

float sdTorus(vec3 p, vec2 t){
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}



//// Sharp boolean operators.

// float min(float a, float b);
// float max(float a, float b);

vec2 mini(vec2 a, vec2 b){
    return a.x < b.x ? a : b;
}

vec2 maxi(vec2 a, vec2 b){
    return a.x < b.x ? b : a;
}


//// Soft boolean operators.
//// (source: IQ, https://iquilezles.org/articles/smin)

float minSoft(float a, float b, float k){
#ifdef NO_SOFT_OPS
    return min(a,b);
#endif
    float h = max( k-abs(a-b), 0.0 );
    return min( a, b ) - h*h*h/(6.0*k*k);
}

float maxSoft(float a, float b, float k){
#ifdef NO_SOFT_OPS
    return max(a,b);
#endif
    float h = clamp( 0.5 - 0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) + k*h*(1.0-h);
}

vec2 miniSoft(vec2 a, vec2 b, float k){
    float s = minSoft(a.x, b.x, k);
    return vec2(s, a.x < b.x ? a.y : b.y);
}

vec2 maxiSoft( vec2 d1, vec2 d2, float k ) {
    float s = maxSoft( d1.x, d2.x, k);
    return vec2(s, d1.x < d2.x ? d2.y : d1.y);
}


//// Helpers
//// Generate a primitive with a custom position and orientation.

float cylinderSmooth(vec3 p, vec3 t, vec3 r, vec2 h){
    vec3 rP = transform(p, t, r);
    return sdRoundedCylinder(rP, h, 0.2);
}

float cylinderFlat(vec3 p, vec3 t, vec3 r, vec2 h){
    vec3 rP = transform(p, t, r);
    return sdCylinder(rP, h);
}

float coneSmooth(vec3 p, vec3 t, vec3 r, vec3 h){
    vec3 rP = transform(p, t, r);
    return sdRoundCone(rP, h.x, h.y, h.z);
}

float coneFlat(vec3 p, vec3 t, vec3 r, vec2 h){
    vec3 rP = transform(p, t, r);
    return sdCone(rP, h.y, h.x);
}

float cubeFlat(vec3 p, vec3 t, vec3 r, vec3 s){
    vec3 rP = transform(p, t, r);
    return sdBox(rP, s);
}

float cubeSmooth(vec3 p, vec3 t, vec3 r, vec3 s, float m){
    vec3 rP = transform(p, t, r);
    return sdSmoothBox(rP, s, m);
}


//// Dragon signed distance function.

vec2 mapFoot(vec3 p){
    // The fingers.
    float footFingers = coneSmooth(p, vec3(0.0), vec3(-0.0, 0.0, -90.0), vec3(0.12, 0.08, 0.3));
    footFingers = min(footFingers, coneSmooth(p, vec3(0.0), vec3(-0.0, 30.0, -90.0), vec3(0.12, 0.08, 0.3)));
    footFingers = min(footFingers, coneSmooth(p, vec3(0.0), vec3(-0.0, -30.0, -90.0), vec3(0.12, 0.08, 0.3)));
    // The nails.
    float footNails = coneSmooth(p, vec3(0.15, 0.0, 0.3), vec3(-0.0, 15.0, -90.0), vec3(0.07, 0.01, 0.15));
    footNails = min(footNails, coneSmooth(p, vec3(0.0, 0.0, 0.35), vec3(-0.0, 0.0, -90.0), vec3(0.07, 0.01, 0.15)));
    footNails = min(footNails, coneSmooth(p, vec3(-0.15, 0.0, 0.28), vec3(-0.0, -15.0, -90.0), vec3(0.07, 0.01, 0.15)));
    // Merge and assign the proper material.
    return mini(vec2(footFingers, 8.0), vec2(footNails, 9.0));
}

vec2 mapJaw(vec3 p){
    // Two cylinders for the jaw itself.
    float jawbase = cylinderFlat(p, vec3(-0.012, -0.128, -0.043), vec3(95.2, -188.0, 7.3 ), vec2(0.048, 0.323));
    jawbase = minSoft(jawbase, cylinderFlat(p, vec3(-0.012, -0.128, 0.178), vec3(-86.0, -173.0, 172.0), vec2(0.048, 0.323)), 0.2);
    // Add a half torus at the front.
    vec3 rP = p - vec3(-0.32, -0.12, 0.02);
    float jawFrontBottom = sdTorus(rP, vec2(0.12, 0.04));
    // Cut it in half by intersecting with a box.
    jawFrontBottom = maxSoft(jawFrontBottom, sdBox(rP + vec3(0.5, 0.0, 0.0), vec3(0.5, 1.0, 1.0)), 0.1);
    // Merge, assign material.
    vec2 jawBottom = vec2(minSoft(jawbase, jawFrontBottom, 0.1), 8.0);
    // The tongue.
    float tongue = cubeFlat(p, vec3(-0.029, -0.105, 0.067), vec3(0.0, -98.6, 0.0), vec3(0.02, 0.001,0.3));
    // Merge with the jaw.
    jawBottom = miniSoft(jawBottom, vec2(tongue, 5.0), 0.14) ;
    // The teeth.
    // Two fangs, using cones.
    float toothBottom = coneFlat(p, vec3(-0.32, -0.058, 0.1), vec3(0.0, -25.2, 0.0), vec2(0.04, 0.08));
    toothBottom = min(toothBottom, coneFlat(p, vec3(-0.3, -0.058, -0.05), vec3(0.0, -25.2, 0.0), vec2(0.04, 0.08)));
    return mini(vec2(toothBottom, 6.0), jawBottom);
}

vec2 mapDragonHead(vec3 p){
    // Bottom jaw.
    vec2 jawBottom = mapJaw(p);
    // Add the pointy tongue.
    vec2 spikingTongue = vec2(coneSmooth(p, vec3(-0.30, -0.12, 0.02), vec3(-54.2, -22.9, 6.23), vec3(0.025, 0.005, 0.2)), 5.0);
    jawBottom = miniSoft(jawBottom, spikingTongue, 0.1);
    // Top jaw.
    vec3 rP = p ;
    // Flip the jaw and orient it.
    rP.y = -rP.y;
    rP.xy = rotate(rP.xy, -0.3);
    vec2 jawtop = mapJaw(rP);
    // Add the nose and chin bumps.
    jawtop = miniSoft(jawtop, vec2(sdSphere(p, vec3(-0.3, 0.25, 0.025), 0.22), 8.0), 0.1);
    jawBottom = miniSoft(jawBottom, vec2(sdSphere(p, vec3(-0.4, -0.2, 0.025), 0.15), 8.0), 0.15);
    vec2 combinedHead = mini(jawBottom, jawtop);
    // Merge both jaws with the base head.
    float baseHead = cubeSmooth(p, vec3(0.163, 0.25, 0.1), vec3(9.41, -7.33, 0.0), vec3(0.2, 0.04, 0.15), 0.07);
    combinedHead = miniSoft(combinedHead, vec2(baseHead, 8.0), 0.3);

    // Spikes on both sides and the neck.
    // Left spikes: a series of rounded cones.
    float leftSpikes = coneSmooth(p, vec3(0.286, 0.236,-0.04), vec3(60.0, -29.1, -4.2), vec3(0.05, 0.01, 0.32));
    leftSpikes = min(leftSpikes, coneSmooth(p, vec3(0.35,0.16,-0.05), vec3(82.6, -26.9, -5.0), vec3(0.056, 0.01, 0.34)));
    leftSpikes = min(leftSpikes, coneSmooth(p, vec3(0.40,0.064,-0.09), vec3(84.1,-30.5,14.1), vec3(0.056,0.01,0.28)));
    leftSpikes = min(leftSpikes, coneSmooth(p, vec3(0.42,-0.037,-0.12), vec3(105.0,-0.0,15.0), vec3(0.056,0.01,0.28)));
    // Same for the right spikes.
    float rightSpikes = coneSmooth(p, vec3(0.286, 0.236,0.22), vec3(60.0, -29.1, 0.0), vec3(0.05, 0.01, 0.32));
    rightSpikes = min(rightSpikes, coneSmooth(p, vec3(0.35,0.16,0.23), vec3(82.6, -26.9, -15.0), vec3(0.056, 0.01, 0.34)));
    rightSpikes = min(rightSpikes, coneSmooth(p, vec3(0.43,0.064,0.27), vec3(84.1,-20.5,-28.0), vec3(0.056,0.01,0.28)));
    rightSpikes = min(rightSpikes, coneSmooth(p, vec3(0.44,-0.037,0.3), vec3(105.0,-10.0,-35.0), vec3(0.056,0.01,0.28)));
    // And the center spikes.
    float centerSpikes = coneSmooth(p, vec3(0.45,0.2,0.1), vec3(70.0,0.0,-0.0), vec3(0.06,0.02,0.5));
    centerSpikes = min(centerSpikes, coneSmooth(p, vec3(0.58,0.3,0.1), vec3(20.0,0.0,-0.0), vec3(0.04,0.01,0.2)));
    // Merge all spikes.
    vec2 spikes = vec2(min(min(leftSpikes, rightSpikes), centerSpikes), 8.0);
    // Add them to the head.
    combinedHead = miniSoft(combinedHead, spikes, 0.1);
    
    // Finally the eyes.
    float eyes = sdSphere(p, vec3(-0.07, 0.25, 0.12), 0.18);
    eyes = min(eyes, sdSphere(p, vec3(-0.05, 0.25, -0.0), 0.18));
    // Pupils.
    float eyeDots = sdSphere(p, vec3(-0.09, 0.26, 0.115), 0.12);
    eyeDots = min(eyeDots, sdSphere(p, vec3(-0.07, 0.26, -0.0), 0.12));
    // Combine everything.
    vec2 mergedEyes = mini(vec2(eyes, 11.0), vec2(eyeDots, 4.0));
    return mini(combinedHead, mergedEyes);;
}

float sdDragon(vec3 p){
	// Body: combine tori and cylinders.
	// Four tori, each intersected with one (or two) boxes to only keep half of it..
    vec3 rP = p - vec3(-0.35, 0.8, -0.9);
    float topbody = sdTorus(rP.yxz, vec2(0.35, 0.2));
    topbody = maxSoft(topbody, sdBox(rP - vec3(0.0, 0.5, 0.0), vec3(1.0, 0.5, 1.0)), 0.15);
    rP = p - vec3(-0.35, 0.25, -0.1);
    float topbody1 = sdTorus(rP.yxz, vec2(0.35, 0.2));
    topbody1 = maxSoft(topbody1, sdBox(rP + vec3(0.0, 0.5, 0.0), vec3(1.0, 0.5, 1.0)), 0.15);
    rP = p - vec3(-0.55, 0.25, -1.45);
    rP.xz = rotate(rP.xz, 0.6);
    float topbody2 = sdTorus(rP.yxz, vec2(0.34, 0.2));
    topbody2 = maxSoft(topbody2, sdBox(rP + vec3(0.0, 0.5, 0.0), vec3(1.0, 0.5, 1.0)), 0.15);
    rP = p - vec3(-0.35, 0.8, 0.5);
    float topbody3 = sdTorus(rP.yxz, vec2(0.34, 0.2));
    topbody3 = maxSoft(topbody3, sdBox(rP - vec3(0.0, 0.5, 0.0), vec3(1.0, 0.5, 1.0)), 0.15);
    topbody3 = maxSoft(topbody3, sdBox(rP - vec3(0.0, 0.5, -1.0), vec3(1.0, 0.5, 1.0)), 0.15);
    // Merge the tori.
    float dragonBody = topbody3;
    dragonBody = min(topbody1, dragonBody);
    dragonBody = min(topbody, dragonBody);
    dragonBody = min(topbody2, dragonBody);
    // Add junction cylinders.
    dragonBody = minSoft(cylinderSmooth(p, vec3(-0.35,0.51,0.2), vec3(0.0, -0, 10.0), vec2(0.105, 0.25)), dragonBody, 0.1);
    dragonBody = minSoft(cylinderSmooth(p, vec3(-0.35,0.55,-0.51), vec3(0.0, -0, 10.0), vec2(0.105, 0.25)), dragonBody, 0.1);
    dragonBody = minSoft(cylinderSmooth(p, vec3(-0.35,0.51,-1.2), vec3(0.0, -0, 10.0), vec2(0.105, 0.25)), dragonBody, 0.1);
    // Smooth a bit the whole body.
    dragonBody -= 0.025;
    // Add the tail.
    dragonBody = minSoft(coneSmooth(p, vec3(-0.75,0.32,-1.74), vec3(30.0, -50.0, -20.0), vec3(0.18, 0.01, 0.8)), dragonBody, 0.1);
    
    // The end of the tail: two flat boxes rotated by 45° and merged.
    vec3 tP = p - vec3(-0.7, 1.2, -1.6);
    tP.xy = rotate(tP.xy, -0.7);
    tP.yz = rotate(tP.yz, 0.4);
    tP.xz = rotate(tP.xz, 0.4);
    float dragonTail = sdSmoothBox(tP, vec3(0.02,0.18, 0.18), 0.04);
    tP.yz = rotate(tP.yz, 0.6);
    dragonTail = min(dragonTail, sdSmoothBox(tP, vec3(0.02,0.18, 0.18), 0.04));
    // Add the end of the tail to the body.
    float dragonMain = minSoft(dragonBody, dragonTail, 0.25);
    
    // Legs.
    // The left back leg has three components because of the weird knee-spike.
    float leftBackLeg = cylinderSmooth(p, vec3(-0.7, 0.298, -1.15), vec3(15.2, 80.0, 46.9), vec2(0.071, 0.296));
    leftBackLeg = min(leftBackLeg, cylinderSmooth(p, vec3(-0.85, 0.2, -1.2), vec3(-5.0, 15.2, -1.0), vec2(0.071, 0.296)));
    leftBackLeg = minSoft(leftBackLeg, coneSmooth(p, vec3(-0.85, 0.7, -1.2), vec3(20.0, 25.0, -1.0), vec3(0.09, 0.03, 0.3)), 0.1);
    // Merge with the foot.
    vec2 leftBackFoot = mini(mapFoot(p - vec3(-0.85, -0.2, -1.2)), vec2(leftBackLeg, 8.0));
    
    // Right back leg, only one cylinder needed.
    float rightBackLeg = cylinderSmooth(p, vec3(-0.2, -0.13, -1.4), vec3(20.0, 20.0, 80.0), vec2(0.071, 0.2));
    vec2 rightBackFoot = mini(mapFoot(p - vec3(-0.1, -0.2, -1.2)), vec2(rightBackLeg, 8.0));
    
    // Right front leg, two cylinders.
    float rightFrontLeg = cylinderSmooth(p, vec3(-0.05, -0.15, -0.1), vec3(20.0, 20.0, 90.0), vec2(0.071, 0.1));
    rightFrontLeg = min(rightFrontLeg, cylinderSmooth(p, vec3(-0.05, 0.05, -0.05), vec3(-90.0, 40.0, 90.0), vec2(0.071, 0.18)));
    vec2 rightFrontFoot = mini(mapFoot(p - vec3(-0.0, -0.2, 0.1)), vec2(rightFrontLeg, 8.0));
    
    // Left front leg: two cylinders.
    float leftFrontLeg = cylinderSmooth(p, vec3(-0.7, -0.0, -0.1), vec3(10.0, -20.0, 110.0), vec2(0.071, 0.1));
    leftFrontLeg = min(leftFrontLeg, cylinderSmooth(p, vec3(-0.65, 0.05, -0.05), vec3(-90.0, 40.0, 90.0), vec2(0.071, 0.18)));
    // For the left front leg, the foot is custom, to wrap it around the blue stone.
    vec3 lP = p - vec3( -0.8, 0.05, 0.1);
    // Fingers.
    float leftFrontFootBase = coneSmooth(lP, vec3(0.0), vec3(-0.0, 0.0, -90.0), vec3(0.12, 0.08, 0.3));
    leftFrontFootBase = min(leftFrontFootBase, coneSmooth(lP, vec3(0.0), vec3(-0.0, 30.0, -90.0), vec3(0.12, 0.08, 0.3)));
    leftFrontFootBase = min(leftFrontFootBase, coneSmooth(lP, vec3(0.0), vec3(-0.0, -30.0, -100.0), vec3(0.12, 0.08, 0.3)));
    // Nails.
    float leftFrontFootNails = coneSmooth(lP, vec3(0.15, 0.0, 0.3), vec3(-0.0, 15.0, -150.0), vec3(0.07, 0.02, 0.15));
    leftFrontFootNails = min(leftFrontFootNails, coneSmooth(lP, vec3(0.0, 0.0, 0.35), vec3(-0.0, 0.0, -150.0), vec3(0.07, 0.02, 0.15)));
    leftFrontFootNails = min(leftFrontFootNails, coneSmooth(lP, vec3(-0.15, -0.08, 0.28), vec3(-0.0, -15.0, -150.0), vec3(0.07, 0.02, 0.15)));
    // Merge the fingers and nails.
    vec2 leftFrontFoot = vec2(min(leftFrontFootBase, leftFrontFootNails), leftFrontFootBase < leftFrontFootNails ? 8.0 : 9.0);
    // Add the leg.
    leftFrontFoot = miniSoft(leftFrontFoot, vec2(leftFrontLeg, 8.0), 0.1);
    // And add a blue sphere under the foot.
    leftFrontFoot = mini(leftFrontFoot, vec2(sdSphere(p, vec3(-0.75, -0.1, 0.3), 0.45), 10.0)); 
    // Merge all feet.
    vec2 feetMerged = mini(mini(mini(leftBackFoot, rightBackFoot),rightFrontFoot), leftFrontFoot);
    
    // Finally the head.
    vec3 headPos = p - vec3(-0.45, 1.2, 0.7);
    headPos.xz = rotate(headPos.xz, M_PI_O_2);
    vec2 dragonHead = mapDragonHead(headPos);

    // Merge everything.
    return miniSoft(miniSoft(vec2(dragonMain, 8.0), feetMerged, 0.2), dragonHead, 0.2).x - 0.15;
}
  
#endif

vec2 iBox(in vec3 ro, in vec3 rd, in vec3 rad)
{
    vec3 m = 1.0 / rd;
    vec3 n = m * ro;
    vec3 k = abs(m) * rad;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    return vec2(max(max(t1.x, t1.y), t1.z),
                min(min(t2.x, t2.y), t2.z));
}

vec2 iSphere( in vec3 ro, in vec3 rd, in vec4 sph )//from iq
{
	vec3 oc = ro - sph.xyz;
	float b = dot( oc, rd );
	float c = dot( oc, oc ) - sph.w * sph.w;
	float h = b * b - c;
	if( h < 0.0 ) return vec2(-1.0);
	h = sqrt(h);
	return vec2(-b - h, -b + h );
}

vec2 opU(vec2 d1, vec2 d2)
{
    return (d1.x < d2.x) ? d1 : d2;
}

vec2 Map(in vec3 pos)
{
    vec2 res = vec2(1e10, 0.0);

    // Ground
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, 1.0, 0.0), 3.0), OBJ_WHITEWALL));
    // Top
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, -1.0, 0.0), 3.0), OBJ_WHITEWALL));
    // Back
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, 0.0,- 1.0), 1.0), OBJ_WHITEWALL));
    // left
    res = opU(res, vec2(sdPlane(pos, vec3(1.0, 0.0, 0.0), 3.0), OBJ_LEFTWALL));
    // Right
    res = opU(res, vec2(sdPlane(pos, vec3(-1.0, 0.0, 0.0), 3.0), OBJ_RIGHTWALL));
    // YuShi
#if OBJ_TYPE == 0
    res = opU(res, vec2(sdSphere(pos, YuShiPos, YushiRadius), OBJ_YUSHI_SPHERE));
#elif OBJ_TYPE == 1
    res = opU(res, vec2(sdSphere(pos, YuShiPos, YushiRadius.x + 2.5), OBJ_YUSHI_SPHERE));
#elif OBJ_TYPE == 2
    res = opU(res, vec2(sdSphere(pos, YuShiPos, YushiRadius + 2.5), OBJ_YUSHI_SPHERE));
#elif OBJ_TYPE == 3
    res = opU(res, vec2(sdSphere(pos, YuShiPos, YushiRadius + 5.5), OBJ_YUSHI_SPHERE));
#endif
    return res;
}

vec2 Map2(in vec3 pos)
{
    vec2 res = vec2(1e10, 0.0);

    // Ground
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, 1.0, 0.0), 3.0), OBJ_WHITEWALL));
    // Top
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, -1.0, 0.0), 3.0), OBJ_WHITEWALL));
    // Back
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, 0.0,- 1.0), 1.0), OBJ_WHITEWALL));
    // left
    res = opU(res, vec2(sdPlane(pos, vec3(1.0, 0.0, 0.0), 3.0), OBJ_LEFTWALL));
    // Right
    res = opU(res, vec2(sdPlane(pos, vec3(-1.0, 0.0, 0.0), 3.0), OBJ_RIGHTWALL));
    return res;
}

vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0) * 0.5773 * 0.0005;
    return normalize( e.xyy * Map2( pos + e.xyy ).x + 
					  e.yyx * Map2( pos + e.yyx ).x + 
					  e.yxy * Map2( pos + e.yxy ).x + 
					  e.xxx * Map2( pos + e.xxx ).x );
}

vec2 RayCast(in vec3 ro, in vec3 rd)
{
    vec2 res = vec2(-1.0, -1.0);
    float tmin = 0.3;
    float tmax = 20.0;

    vec2 tb = iBox(ro, rd, BOX_SIZE);
    if(tb.x < tb.y && tb.y > 0.0 && tb.x < tmax)
    {
        tmin = max(tb.x, tmin);
        tmax = min(tb.y, tmax);

        float t = tmin;
        for(int i = 0; i < 70 && t < tmax; ++i)
        {
            vec2 h = Map(ro + rd * t);
            if(abs(h.x) < (0.0001 * t))
            {
                res = vec2(t, h.y);
                break;
            }
            t += h.x;
        }
        return res;
    }
    return res;
}

vec2 RayCast2(in vec3 ro, in vec3 rd)
{
    vec2 res = vec2(-1.0, -1.0);
    float tmin = 0.3;
    float tmax = 60.0;

    vec2 tb = iBox(ro, rd, BOX_SIZE);
    if(tb.x < tb.y && tb.y > 0.0 && tb.x < tmax)
    {
        tmin = max(tb.x, tmin);
        tmax = min(tb.y, tmax);

        float t = tmin;
        for(int i = 0; i < 70 && t < tmax; ++i)
        {
            vec2 h = Map2(ro + rd * t);
            if(abs(h.x) < (0.0001 * t))
            {
                res = vec2(t, h.y);
                break;
            }
            t += h.x;
        }
        return res;
    }
    return res;
}

float GetYushiColor(in vec3 p) {
	
	float res = 0.;
	
    vec3 c = p;
	for (int i = 0; i < 10; ++i) 
    {
        p =.7 * abs(p) / dot(p,p) - .7;
        p.yz = csqr(p.yz);
        p = p.zxy;
        res += exp(-19. * abs(dot(p,c)));
        
	}
	return res / 2.;
}

void getParticipatingMedia(out vec3 sigmaS, out vec3 sigmaE, in vec3 pos)
{
#if ROTATE_MEDIA == 1
    pos = (pos - YuShiPos) * rotate_around_y(180 * sin(iTime * rotateSpeed)) + YuShiPos;
#endif

#if OBJ_TYPE == 0
    float sphereFog = clamp((YushiRadius - length(pos - YuShiPos)) / YushiRadius, 0.0, 1.0);
#elif OBJ_TYPE == 1
    //pos = (pos - YuShiPos) * rotate_around_y(180 * sin(iTime * rotateSpeed)) + YuShiPos;
    float sphereFog = clamp(sdTorus(pos, YuShiPos, YushiRadius) * -3.2, 0.0, 1.0);
#elif OBJ_TYPE == 2
    float sphereFog = clamp(sdOctahedron(pos, YuShiPos, YushiRadius) * -3.2, 0.0, 1.0);
#elif OBJ_TYPE == 3
    pos = (pos + vec3(-0.9, 1.0, 0.0) - YuShiPos) * rotate_around_y(90) + YuShiPos;
    float sphereFog = clamp(sdDragon(pos) * -3.2, 0.0, 1.0);
#endif
    
    sigmaS = sphereFog * SCATTERING;
    sigmaE = ABSORPTION + sigmaS;

#if BASIC_ANIMATED_MEDIA==1
    float r = floor(iTime);
    sigmaS = sphereFog * abs(5.0* vec3(rand(vec3(r,0.0,1.0)),rand(vec3(r,0.0,5.0)),rand(vec3(r,0.0,9.0))));
    vec3 absorption = abs(0.0* vec3(rand(vec3(r,1.0,2.0)),rand(vec3(r,1.0,7.0)),rand(vec3(r,1.0,7.0))));
    sigmaE = sigmaS + absorption;
#elif BASIC_ANIMATED_MEDIA==2
    float r = iTime*0.2;
    sigmaS = sphereFog * abs(5.0* vec3(sin(r*1.1),sin(r*3.3),sin(r*5.5)));
    vec3 absorption = abs( 0.1* vec3(sin(r*2.2),sin(r*4.4),sin(r*6.6)));
    sigmaE = sigmaS + absorption;
#endif

    sigmaE = max_f_v3(0.0000001, sigmaE);
}

float volumetricShadow(in vec3 from, in vec3 to)
{
#ifndef IS_SHOW_SHADOW
    return 1.0;
#endif

#ifdef IS_SHOW_SHADOW
#if IS_SHOW_SHADOW == false
    return 1.0;
#else
    const float numStep = MAX_SHADOW_STEP_NUM;  // quality control. Bump to avoid shadow alisaing
    vec3 shadow = vec3(1.0);
    vec3 sigmaS = vec3(0.0);
    vec3 sigmaE = vec3(0.0);
    float dd = length(to-from) / numStep;
    for(float s = 0.5; s < (numStep - 0.1); s += 1.0)// start at 0.5 to sample at center of integral part
    {
        vec3 pos = from + (to - from) * (s / (numStep));
        getParticipatingMedia(sigmaS, sigmaE, pos);
        shadow = shadow * exp(-sigmaE * ShadowIntensity * dd);
    }
    return length(shadow) * 0.8;
#endif
#endif
    //return max_v3_elem(shadow);
}


vec3 evaluateLightWithPhase(in vec3 pos)
{
    vec3 light_sum = vec3(0.0);
    vec3 V = normalize(pos - viewPosWS);
    for(int i = 0; i < pointLightsNum; ++i)
        light_sum += GetPointLightIllumiance(i, pos) * 
        PhaseFunc(V, normalize(pointLights[i].position - viewPosWS), PHASE_G) * 
        volumetricShadow(pos, pointLights[i].position);
        
    
    return light_sum;
}


vec3 GetColor(in float ID, in vec3 ro, in vec3 rd, inout vec4 pre_pos, inout vec4 pre_scat, inout vec4 pre_transmittance)
{
    if(ID < 3.5)
    {
        pre_scat = vec4(0.0, 0.0, 0.0, 1.0);
        pre_transmittance = vec4(1.0, 1.0, 1.0, 0.0);
    }
    if(ID < 0.5) return vec3(0.0);
    if(ID < 1.5) return WhiteWallColor;
    if(ID < 2.5) return RightWallColor;
    if(ID < 3.5) return LeftWallColor;
    else
    {
#if OBJ_TYPE == 0
        vec2 tmm = iSphere(ro, rd, vec4(YuShiPos, YushiRadius));
#elif OBJ_TYPE == 1
        vec2 tmm = iSphere(ro, rd, vec4(YuShiPos, max(YushiRadius.x, YushiRadius.y) + 0.5));
#elif OBJ_TYPE == 2
        vec2 tmm = iSphere(ro, rd, vec4(YuShiPos, YushiRadius + 0.5));
#elif OBJ_TYPE == 3
        vec2 tmm = iSphere(ro, rd, vec4(YuShiPos, YushiRadius));
#endif
        float t = tmm.x;
        float stepsNum = MAX_STEP_NUM;
        float dt = (tmm.y - tmm.x) / stepsNum;     //float dt = .2 - .195*cos(iTime*.05);
        vec3 transmittance = pre_transmittance.xyz;
        vec3 scatteredLight = pre_scat.xyz;
        vec3 sigmaS = vec3(0.0);
        vec3 sigmaE = vec3(0.0);

        float pre_t = length(pre_pos.xyz - ro);
        t = max(pre_t, t);
        //t = t + dt * FrameIndex;

        /*if(pre_pos.w == 1.0 && t > tmm.y)
        {
            t = tmm.x;
            pre_pos.w = 0.0;
            transmittance = vec3(1.0, 1.0, 1.0);
            scatteredLight = vec3(0.0);
        }*/
        stepsNum *= TEMPORAL_ACCELERATION_MUL;
      
        for( int i = 0; i < stepsNum; ++i )
        {
             if(t > tmm.y)
            { 
                //pre_pos.w = 1.0;
                break;
            }           
    
            vec3 p = ro + t * rd;
            getParticipatingMedia(sigmaS, sigmaE, p);

            vec3 S = evaluateLightWithPhase(p) * sigmaS;
            vec3 sigmaE_INV = vec3(1.0 / sigmaE.x, 1.0 / sigmaE.y, 1.0 / sigmaE.z); 
            vec3 Sint = (S - S * exp(-sigmaE * dt)) * sigmaE_INV;
            scatteredLight += transmittance * Sint; // accumulate and also take into account the transmittance from previous steps
            transmittance *= exp(-sigmaE * dt);
                    
            t += dt;


        }

        pre_pos = vec4(ro + t * rd, pre_pos.w);
        pre_scat = vec4(scatteredLight, 0.0);
        pre_transmittance = vec4(transmittance, 0.0);

        return vec3(0.0);
    }
}

vec3 GetColor2(in float ID, in vec3 ro, in vec3 rd)
{
    if(ID < 0.5) return vec3(0.0);
    if(ID < 1.5) return WhiteWallColor;
    if(ID < 2.5) return RightWallColor;
    if(ID < 3.5) return LeftWallColor;
    return WhiteWallColor;
}

bool HistoryRejection(in vec3 N1)
{
    float FOVEA_Factor = 1.0 - Map_HSV(HSV_PDF(degrees(acos(dot(N1, view_center_dir))))); /* 值越小，越触发 */
    const float Const_Factor = 0.05;
    //return F_ANG > 1.01;
    //return F_ANG + (FOVEA_Factor * Const_Factor)  > 1.04;
    // F_ANG / (FOVEA_Factor * Const_Factor) < 1.0;
    return F_DIS > 1.2 || F_ANG > 1.01;
}

void main()
{
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * (vec4(ReMapNDC_V3(vec3(TexCoords, 1.0)), 1.0) * far_plane);
    vec3 dir  = normalize(rd.xyz - orig);
    float W = rd.w;

    // Read history
    vec4 pos_tex;
    vec4 scat_tex;
    vec4 trans_tex;
    //vec4 flags = vec4(1e-5);

    // Ray Marching
    vec2 res = RayCast(orig, dir);
    vec2 res2 = RayCast2(orig, dir);
    if(res.x < 0.0001) res.x = far_plane;
    vec3 pos = orig + dir * res.x;
    vec3 pos2 = orig + dir * res2.x;
    vec3 N = calcNormal(pos2);

    // Read History Data
    vec3 M_pos = pos;
    vec4 M_ndc = projection_pre * view_pre * vec4(M_pos, 1.0);
    M_ndc = M_ndc / M_ndc.w;
    vec2 M_uv = M_ndc.xy * 0.5 + 0.5;
    vec3 M_dir = normalize(M_pos - LastViewPosWS);

    // Solve History Data
    bool isFirstFrame = FrameIndex == 0;
    pos_tex = mix(texture(FinalPos_In, M_uv), vec4(orig, 0.0), isFirstFrame);
    scat_tex = mix(texture(ScatterLight_In, M_uv), vec4(0.0, 0.0, 0.0, 1.0), isFirstFrame);
    trans_tex = mix(texture(Transmittance_In, M_uv), vec4(1.0, 1.0, 1.0, 1.0), isFirstFrame);
    
    bool isOutRange = inRange(M_uv) < -0.05;
    pos_tex = mix(pos_tex, vec4(orig, 0.0), isOutRange);
    scat_tex = mix(scat_tex, vec4(0.0, 0.0, 0.0, 1.0), isOutRange);
    trans_tex = mix(trans_tex, vec4(1.0, 1.0, 1.0, 1.0), isOutRange);

    bool isReStart = HistoryRejection(dir);
    float blend_op = isReStart ? 0.6 : 0.0;
    pos_tex = mix(pos_tex, vec4(orig, 0.0), isReStart);
    scat_tex = mix(scat_tex, vec4(0.0, 0.0, 0.0, 1.0), blend_op);
    trans_tex = mix(trans_tex, vec4(1.0, 1.0, 1.0, 1.0), blend_op);

    bool isRefresh = FrameIndex % 200 == 0;
    pos_tex = mix(pos_tex, vec4(orig, 0.0), isRefresh);
    scat_tex = mix(scat_tex, vec4(0.0, 0.0, 0.0, 1.0), isRefresh);
    trans_tex = mix(trans_tex, vec4(1.0, 1.0, 1.0, 1.0), isRefresh);

    // Compute ScatterLight
    vec3 baseColor = GetColor(res.y, orig, dir, pos_tex, scat_tex, trans_tex);
    vec3 baseColor2 = GetColor2(res2.y, orig, dir);

    // Light Compute
    vec3 diffuse_color = BaseColorReMap(baseColor2, WALL_METALLIC);
	float r = RoughnessReMap(WALL_ROUGHNESS);
	vec3 f0 = GetF0_All(baseColor, WALL_METALLIC, WALL_REFLECTANCE);
    vec3 result = vec3(0.0);
    for(int i = 0; i < pointLightsNum; ++i)
	{
		//light_Intensity * light_color * BRDF * NoL 
		vec3 L = normalize(pointLights[i].position - pos2);
		float NoL = clamp(dot(N, L), 0.001, 1.0);
		result = result + GetPointLightColor(i) * GetPointLightIllumiance(i, pos2)
		* Standard_BRDF_Torrance_DVF(diffuse_color, dir, L, N, r, f0, 1.0) 
        * NoL * volumetricShadow(pos2, pointLights[i].position);
	}	

    // Blend
    result = scat_tex.xyz + result *  length(trans_tex);
    // result = scat_tex.xyz + result *  max_v3_elem(trans_tex.xyz);

    // Show Light
    for(int i = 0; i < pointLightsNum; ++i)
	{
        vec2 re = iSphere(orig, dir, vec4(pointLights[i].position, LIGHT_SIZE));
        bool isLight = re.x < re. y && re.y > 0.0;
        result = mix(result, LIGHT_COLOR, isLight);
    }

    // Color Solve
    result = result/ (result + vec3(1.0));
    result = GammaCorrection(result);

    // Output
    FragColor = vec4(result, 1.0);
    ScatterLight_Out = scat_tex;
    Transmittance_Out = trans_tex;
    FinalPos_Out = pos_tex;

    //FragColor = vec4(baseColor, 1.0);
    //FragColor = texture(FinalPos_In, M_uv);
    //FragColor = vec4(vec3(dot(M_dir, dir) * 10.0), 1.0);
    //FragColor = scat_tex;
    //FragColor = vec4(FrameIndex / 1000.0);
    //FragColor = scat_tex;
    //FragColor = trans_tex;
    //if(isReStart)FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //if(isRefresh)FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //FragColor = vec4(1.0 - Map_HSV(HSV_PDF(degrees(acos(dot(dir, view_center_dir))))));
}


