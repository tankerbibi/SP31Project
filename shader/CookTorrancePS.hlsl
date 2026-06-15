#include "Common.hlsl"
#include "CookTorranceSub.hlsl"

Texture2D g_Texture : register(t0);
// テクスチャを取り出してくるハードウェア
SamplerState g_Sampler : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    // ピクセルから光源へのベクトル（いつもと逆）
    float4 lv = Light.Position - In.WorldPosition;
    //　物体と光源の距離
    float4 ld = length(lv);
    // ベクトルの正規化
    lv = normalize(lv);
    // 減衰の計算
    float ofs = 1.0 - (1.0 / Light.PointLightparam.x) * ld;
    //　減衰率0未満は0にする。
    ofs = max(0, ofs);
    // ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);
    // 光源計算
    float light = 0.5 + 0.5 * dot(normal.xyz, lv.xyz);
    light = saturate(light);
    //　明るさを減衰させる
    light *= ofs;
    // テクスチャのピクセル色を取得
    float4 albedoColor = g_Texture.Sample(g_Sampler, In.TexCoord);
    
    outDiffuse = albedoColor;
    // 明るさを乗算
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    // アルファ値に明るさは関係ない
    outDiffuse.a *= In.Diffuse.a;
    
    // ピクセルからカメラへ向かうベクトル（いつもと逆）
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);
    // ハーフベクトルを計算
    // 視線ベクトル + ライトベクトル
    float3 halfv = eyev + lv.xyz;
    halfv = normalize(halfv);
    // スペキュラーの計算
    float3 specular;
    float nv = saturate(dot(normal.xyz, eyev));
    float nh = saturate(dot(normal.xyz, halfv));
    float vh = saturate(dot(eyev, halfv));
    float nl = saturate(dot(normal.xyz, lv.xyz));
    
    //　今回はテクスチャからの情報でなく設定した数値で代用
    float smooth = saturate(Parameter.x);
    float metallic = saturate(Parameter.y);
    
    // 面の粗さ
    float D = CalculateBeckmann(smooth, nh);
    float G = CalculateGeometricDamping(nh, nv, nl, vh);
    float F = calculateFresnel(metallic, dot(lv.xyz, halfv));
    
    specular = max(0.0, F * D * G / nv) * albedoColor.rgb;
    
    
    // = のみにしてスペキュラだけにすると分かりやすい
    outDiffuse.rgb += (specular * ofs);
}
