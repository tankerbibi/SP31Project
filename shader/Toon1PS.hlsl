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
    
    // 明るさの調整
    if(light > Parameter.y)
    {
        light = 1.0f;
    }
    else if(light.x > Parameter.x)
    {
        light = 0.7f;
    }
    else
    {
        light = 0.4f;
    }
    
    
    // 明るさを減衰する
    light *= ofs;
    // テクセルを取得
    outDiffuse = g_Texture.Sample(g_Sampler, In.TexCoord);
    // テクセルに明るさを乗算する(aはかけちゃダメ。なぜなら、aは色ではないから。)
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;
    ///////////////////////////////////////////
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
