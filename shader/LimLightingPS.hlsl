#include "Common.hlsl"

Texture2D g_Texture : register(t0);
// テクスチャを取り出してくるハードウェア
SamplerState g_Sampler : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    //　光源からピクセルへノベクトル
    float4 lv = In.WorldPosition - Light.Position;
    // 物体と光源の距離
    float4 ld = length(lv);
    // ベクトルの正規化
    lv = normalize(lv);
    // 減衰の計算
    float ofs = saturate(1.0f - ld / Light.PointLightparam.x);
    // 減衰率０未満は０にする
    ofs = max(0.0f, ofs);
    ///////////////////////////////////////////
    // 法線を正規
    float4 normal = normalize(In.Normal);
    // 光源計算
    float light = -dot(normal.xyz, lv.xyz);
    // lightをサチュレート
    light = saturate(light);
    // 明るさを減衰する
    light *= ofs;
    // テクセルを取得
    outDiffuse = g_Texture.Sample(g_Sampler, In.TexCoord);
    // テクセルに明るさを乗算する(aはかけちゃダメ。なぜなら、aは色ではないから。)
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;
    ///////////////////////////////////////////
    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    //　ハーフベクトルを計算 反射ベクトルを使わないのがポイント
    float3 halfv = eyev + lv.xyz;
    halfv = normalize(halfv);
    
    // 鏡面反射の計算
    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    //↓の強さによっていろいろ変わるらしい。 光が強いところのみが白くなる。
    specular = pow(specular, 30);

    outDiffuse.rgb += (specular * ofs);
    /////////////////////////////////////
    
    
    // リムライティングの処理
    // 光の方向と視線ベクトルの考慮
    float lit;
    // 逆光なほど1になる。
    lit =  1.0f - max(0.0f, dot(lv.xyz, eyev));
    // 輪郭部分ほど明るくなるようにする
    float lim;
    lim = 1.0f - max(0.0f, dot(normal.xyz, -eyev));;
    // litとlimの明るさを合成
    lim *= lit;
    // 適当に何乗とかして調整する。
    lim = pow(lim, Light.PointLightparam.y);
    // リムライティングの明るさを加算
    outDiffuse.rgb += lim;
    
    
}

