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
    // 法線を回転させる
    worldNormal = mul(normal, World);
    // light２頂点の明るさができる
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;

    // 明るさの出力
    Out.Diffuse = In.Diffuse;
    Out.TexCoord = In.TexCoord;
    
    
    Out.WorldPosition = mul( In.Position, World);
}
