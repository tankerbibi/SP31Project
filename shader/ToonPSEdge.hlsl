#include "Common.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_TextureRamp : register(t1);
// テクスチャを取り出してくるハードウェア
SamplerState g_Sampler : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    outDiffuse = In.Diffuse;
}
