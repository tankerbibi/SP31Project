#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

float Rand(float2 coord)
{
    return frac(sin(dot(coord.xy, float2(12.9898f, 78.233f))) * 43758.5453f);

}

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // テクセルの取得
    float4 Tex = g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    // Parameter.y ~ Parameter.zの乱数値を作成
    float noise = clamp(Rand(In.TexCoord * Parameter.x), Parameter.y, Parameter.z);
    
    // 乱数値を輝度としてテクセルへ加算
    Tex.rgb += noise;
    
    // 頂点の色合成
    outDiffuse.rgb = saturate(Tex.rgb) * In.Diffuse.rgb;
    // 頂点のα値を合成
    outDiffuse.a = Tex.a * In.Diffuse.a;
    
    //　グレースケール化
    outDiffuse.rgb = (outDiffuse.r * 0.3f) + (outDiffuse.g * 0.6f) + (outDiffuse.b * 0.1f);
    // コントラスト調整
    outDiffuse.rgb = pow(outDiffuse.rgb, Parameter.w);
}