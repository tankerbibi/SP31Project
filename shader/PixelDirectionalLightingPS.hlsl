#include "Common.hlsl"

Texture2D g_Texture : register(t0);
// テクスチャを取り出してくるハードウェア
SamplerState g_Sampler : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    // 法線を正規
    float4 normal = normalize(In.Normal);
    // 光源計算
    float light = -dot(normal.xyz, Light.Direction.xyz);
    light = saturate(light);
    // テクセルを取得
    outDiffuse = g_Texture.Sample(g_Sampler, In.TexCoord);
    // テクセルに明るさを乗算する
    outDiffuse.rgb *= In.Diffuse.rgb * light;
    outDiffuse.a = In.Diffuse.a;
    
    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    // 光の反射ベクトルを計算
    float3 refv = reflect(Light.Direction.xyz, normal.xyz);
    refv = normalize(refv);
    
    // 鏡面反射の計算
    float specular = -dot(eyev, refv);
    specular = saturate(specular);
    //↓の強さによっていろいろ変わるらしい。 光が強いところのみが白くなる。
    specular = pow(specular, 30);

    outDiffuse.rgb += specular;
}
