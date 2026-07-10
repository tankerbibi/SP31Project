#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    
    float4 pos = In.Position;
    float4 norm = normalize(In.Normal) * Parameter.z;
    norm.w = 0.0f;
    pos += norm;
    Out.Position = mul(pos, wvp);
    
    // 光源計算　PDFの１７ページからの処理
    float4 worldNormal;
    float4 normal;
    
    normal = float4(In.Normal.xyz, 0.0);
    // 法線を回転させる
    worldNormal = mul(normal, World);
    // light二兆点の明るさができる
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;

    // 頂点色
    Out.Diffuse.rgb = In.Diffuse.rgb * 0.2f;
    Out.Diffuse.a = 1.0f;
    // テクスチャ座標の出力
    Out.TexCoord = In.TexCoord;
    
    // ワールド変換だけした頂点座標を出力
    Out.WorldPosition = mul( pos, World);
}
