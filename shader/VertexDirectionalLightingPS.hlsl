#include "Common.hlsl"

Texture2D g_Texture : register(t0);
// テクスチャを取り出してくるハードウェア
SamplerState g_Sampler : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    outDiffuse = g_Texture.Sample(g_Sampler, In.TexCoord);
    // ピクセルの色を合成する
    outDiffuse *= In.Diffuse; // outDiffuse.rgba *= In.Diffuse.rgba;←これと同じ！
}
