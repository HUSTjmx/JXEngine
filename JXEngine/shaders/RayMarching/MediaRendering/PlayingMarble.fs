#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 LastColor;
layout(location = 2) out vec4 ScatterLight;
layout(location = 3) out vec4 FinalPos;

in vec2 TexCoords;

uniform mat4 view;
uniform mat4 view_inv;
uniform mat4 view_pre;
uniform mat4 projection;
uniform mat4 projection_inv;
uniform mat4 projection_pre;

uniform sampler2D preTexture;
uniform sampler2D scatterTex;
uniform sampler2D posTex;

uniform int IsFirstFrame;

float zoom = 1.0;

vec2 cmul( vec2 a, vec2 b )  { return vec2( a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x ); }

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

float map(in vec3 p) {
	
	float res = 0.;
	
    vec3 c = p;
	for (int i = 0; i < 10; ++i) 
    {
        p =.7 * abs(p) / dot(p,p) - .7;
        p.yz= csqr(p.yz);
        p = p.zxy;
        res += exp(-19. * abs(dot(p,c)));
        
	}
	return res / 2.;
}

void raymarch( vec3 ro, vec3 rd, vec2 tminmax, inout vec4 pos, inout vec3 col)
{
    float t = tminmax.x;
    float dt = .02;
    //float dt = .2 - .195*cos(iTime*.05);//animated
    float c = map(pos.xyz);
    if(IsFirstFrame == 0) c = 0.0;
    vec3 tt = (pos.xyz - ro) / rd;

    t = tt.x;

    for( int i = 0; i < 10; i++ )
	{
        t += dt * exp(-2. * c);
        if(t > tminmax.y)
        {
            t -= dt * exp(-2. * c);
            break;
        }
                
        c = map(ro + t * rd);               
        
        col = .99 * col + .08 * vec3(c * c * c, c * c * c, c * c);//green	
        //col = .99*col+ .08*vec3(c*c*c, c*c, c);//blue
    }
    //col = tt;
    pos.xyz = ro + t * rd;
}

void main()
{
    vec4 o_pos;
    vec3 col;

    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * vec4(vec3(ReMapNDC_V2(TexCoords), 1.0), 1.0);
    vec3 dir = normalize(rd.xyz);

    vec4 pre_pos = projection_pre * view_pre * rd;
    pre_pos = pre_pos / pre_pos.w;
    vec2 uv = pre_pos.xy * 0.5 + 0.5;

    if(IsFirstFrame == 0)
    {
        o_pos = vec4(orig, 0.0);
        col = vec3(0.0, 0.0, 0.0);
    }
    else{
        o_pos = texture(posTex, uv);
        col = texture(scatterTex, uv).xyz;
    }

    vec2 tmm = iSphere(orig, dir, vec4(0.,0.,0.,2.));
    raymarch(orig, dir, tmm, o_pos, col);
    vec3 bg = vec3(1.0, 0.5, 0.5);
    
    ScatterLight.xyz = col;
    if (tmm.x < 0.)
    {
        col = bg;
        ScatterLight.xyz = col;
    }
    else
    {
        vec3 nor = (orig + tmm.x * dir) / 2.0;
        nor = reflect(dir, nor);        
        float fre = pow(0.5 + clamp(dot(nor, dir), 0.0, 1.0), 3.0) * 1.3;
        col += bg * fre;
    }

    FinalPos = o_pos;
    LastColor.xyz = col;
    
    col = 0.5 * (log(1.0 + col));
    col = clamp(col, 0.0, 1.0);
    FragColor = vec4(col, 1.0);
    
    //FragColor = vec4(o_pos.xyz, 1.0);
    //if(IsFirstFrame % 10 < 5)
    //{
    //    FragColor = vec4(TexCoords, 1.0 , 1.0);
    //}
    //else
    //{
    //    FragColor = vec4(uv, 0.0, 1.0);
    //}
    // FragColor = vec4(TexCoords - uv, 0.0, 1.0);
   // FragColor = vec4(FinalPos.xyz, 1.0);
    //FragColor = vec4(texture(posTex, uv).xyz, 1.0);
    //FragColor = vec4(dir, 1.0);
}