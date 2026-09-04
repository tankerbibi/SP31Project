#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main (in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float2 uv = In.TexCoord;
    
    // 全画面モザイク
    {
        uv *= Parameter.xy;  // UV座標に解像度を乗算
        uv /= Parameter.z;  // モザイクサイズで除算
        uv = floor(uv) * Parameter.z;  // モザイクサイズを乗算
        uv /= float2(Parameter.x, Parameter.y);  // 解像度で除算
    }
    
    outDiffuse = g_Texture.SampleLevel(g_SamplerState, uv, 0.0f);
    outDiffuse *= In.Diffuse;
}