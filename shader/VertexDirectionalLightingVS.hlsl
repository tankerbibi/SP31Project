#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);
    
    // 光源計算　PDFの１７ページからの処理
    float4 worldNormal;
    float4 normal;
    
    normal = float4(In.Normal.xyz, 0.0);
    normal.xyz = In.Normal.xyz;
    normal.w = 0.0;
    // 法線を回転させる
    worldNormal = mul(normal, World);
    // light二兆点の明るさができる
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;
    // 光ベクトルと法線の内積計算
    float light = 0.5 - 0.5 * dot(Light.Direction.xyz, worldNormal.xyz);
    // 明るさを0.0~1.0にする
    light = saturate(light);
    // 明るさの出力
    Out.Diffuse.rgb = light * In.Diffuse;
    Out.Diffuse.a = In.Diffuse.a;
    // α値の出力
    Out.TexCoord = In.TexCoord;
    

}
