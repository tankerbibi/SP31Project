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
    float4 col = g_Texture.Sample(g_SamplerState, In.TexCoord);
    // 頂点カラー乗算
    col.rgb *= In.Diffuse.rgb;
    // コントラスト
    col.rgb = pow(saturate(col), Parameter.w);
    // ノイズ
    col.rgb += clamp(Rand(In.TexCoord * 300.0f), Parameter.y, Parameter.z);
    
    // ポスタライズ
    float div = 1.0f / Parameter.x;  // 一段階当たりの幅
    col.rgb = floor(col / div) * div; // 幅の値を使って色の値を求める
    col.rgb = saturate(col);
    // 結果出力
    outDiffuse.rgb = col.rgb;
    // アルファ値の処理
    outDiffuse.a = col.a * In.Diffuse.a;
}
