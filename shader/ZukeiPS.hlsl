
#include "Common.hlsl"

Texture2D g_Texture : register(t0);	//0”ÔƒeƒNƒXƒ`ƒƒ
SamplerState g_SamplerState : register(s0);

//@“h‚è‚Â‚Ô‚µ‚Ì‰~
float circle(float2 tex, float rad)
{
    return length(tex) - rad;
}

// ƒ‰ƒCƒ“•`‰æ‚Ì‰~
float ring(float2 tex, float rad, float w)
{
    w = rad - w;
    return abs(length(tex) - rad) + w;
}

// ƒn[ƒgŒ^
float heart(float2 tex, float size)
{
    float hekomi = Parameter.w;
    tex.x = 2.0f * tex.x - sign(tex.x) * (-tex.y) * hekomi;
    
    return length(tex) - size;
}

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float ASPECT = (1280.0f / 720.0f);
    //{  // “h‚è‚Â‚Ô‚µ‚Ì‰~
    //    float loop = 1.0f;
    //    float2 tex = frac(In.TexCoord * loop) * 2.0f - 1.0f;
    //    tex.x *= ASPECT;
        
    //    float rad = Parameter.x;
    //    float col = circle(tex,rad);

    //    col = smoothstep(rad, rad + Parameter.y, col);
        
    //    outDiffuse.rgb = 1.0f - col;
    //    outDiffuse.a = 1.0f;
    //}
    
    //{  // ƒ‰ƒCƒ“•`‰æ‚Ì‰~
    //    float loop = 1.0f;
    //    float2 tex = frac(In.TexCoord * loop) * 2.0f - 1.0f;
    //    tex.x *= ASPECT;
        
    //    float rad = Parameter.x;
    //    float w = Parameter.z;
        
    //    float col = ring(tex, rad, w);

    //    col = smoothstep(rad, rad + Parameter.y, col);
        
    //    outDiffuse.rgb = 1.0f - col;
    //    outDiffuse.a = 1.0f;
    //}

    {  // ƒn[ƒgŒ^
        float loop = 1.0f;
        float2 tex = frac(In.TexCoord * loop) * 2.0f - 1.0f;
        tex.x *= ASPECT;
        
        float size = Parameter.x;
        
        float col = heart(tex, size);

        col = smoothstep(size, size + Parameter.y, col);
        
        outDiffuse.rgb = 1.0f - col;
        outDiffuse.a = 1.0f;
    }
}


