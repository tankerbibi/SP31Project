#include "Common.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_TextureNormal : register(t1);
// テクスチャを取り出してくるハードウェア
SamplerState g_Sampler : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    // 光源からピクセルへのベクトル
    float4 lv = In.WorldPosition - Light.Position;
    // 物体と光源の距離
    float4 ld = length(lv);
    // ベクトルの正規化
    lv = normalize(lv);
    
    // 減衰の計算
    float ofs = saturate(1.0f - ld / Light.PointLightparam.x);
    
    // 法線マップ取得
    float4 tmpnormal = g_TextureNormal.Sample(g_Sampler, In.TexCoord);;
    tmpnormal = normalize((tmpnormal * 2.0f) - 1.0f);
    
    float3 normal;
    normal = tmpnormal.xyz;
    
    float3 T = float3(1.0f, 0.0f, 0.0f);
    float3 B = float3(0.0f, 0.0f, 1.0f);
    float3 N = float3(0.0f, 1.0f, 0.0f);
    // TBN回転行列
    float3x3 Rot = float3x3(T, B, N);
    // 法線マップのベクトルをTBNで回転
    normal = normalize(mul(normal, Rot));
    // 明るさを計算
    float light = -dot(normal.xyz, lv.xyz);
    light = saturate(light);
    // 明るさを減衰させる
    light *= ofs;
    
    outDiffuse = g_Texture.Sample(g_Sampler, In.TexCoord);
    outDiffuse.rgb *= Light.Diffuse.rgb * In.Diffuse.rgb * light + Light.Ambient.rgb;
    // アルファ値に明るさは関係ない
    outDiffuse.a *= In.Diffuse.a;
    // 視線ベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    // ハーフベクトル
    float3 halfv = eyev + lv.xyz;
    halfv = normalize(halfv);
    // スペキュラーを計算
    // ハーフベクトルと法線の内積
    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, 30.0f);
    
    // スペキュラも減衰させてから加算して出力
    outDiffuse.rgb += (specular * ofs);
}
