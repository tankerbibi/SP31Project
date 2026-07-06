#include "Common.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_TextureRamp : register(t1);
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
    // lightをクランプ テクスチャ座標(U)
    light = clamp(light, 0.01f, 0.99f) + Parameter.y;

    float texv = Parameter.x;
    // テクスチャ座標Y座標をクランプ
    texv = clamp(texv, 0.01f, 0.99f);
    // テクスチャから明るさを取得
    float4 toon = g_TextureRamp.Sample(g_Sampler, float2(light, texv));
    // 明るさを減算する
    toon *= ofs;
    
    //　テクスチャから入りを取得
    outDiffuse = g_Texture.Sample(g_Sampler, In.TexCoord);
    outDiffuse.rgb *= toon.rgb * In.Diffuse.rgb * Light.Diffuse.rgb + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse;
    
    //// 明るさを減衰する
    //light *= ofs;
    //// テクセルを取得
    //outDiffuse = g_Texture.Sample(g_Sampler, In.TexCoord);
    //// テクセルに明るさを乗算する(aはかけちゃダメ。なぜなら、aは色ではないから。)
    //outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    //outDiffuse.a *= In.Diffuse.a;
    ///////////////////////////////////////////
    //　簡易エッジを作成
    float4 eyev = In.WorldPosition - CameraPosition;
    eyev = normalize(eyev);
    // 視線とピクセル法線の内積
    float d = dot(normal, eyev);
    if(d > Parameter.z)
    {
        // 明度を落とす
        outDiffuse.rgb *= 0.3f;
    }
}
