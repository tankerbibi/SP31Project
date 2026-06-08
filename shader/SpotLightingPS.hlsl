#include "Common.hlsl"

Texture2D g_Texture : register(t0);
// テクスチャを取り出してくるハードウェア
SamplerState g_Sampler : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float spot = 0.0f;
    float light = 0.0f;
    float specular = 0.0f;
    
    // ライトーーーーーー＞ピクセル
    float3 Vp = normalize(In.WorldPosition.xyz - Light.Position.xyz);
    float angle = acos(dot(Vp.xyz, Light.Direction.xyz));
    {
        //　中心に近いほど明るくなる値を計算。
        spot = saturate(1.0f - pow(1.0f / Light.Angle.x * abs(angle), Light.PointLightparam.y));

        float4 normal = normalize(In.Normal);
        light = -dot(normal.xyz, Vp);
        float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
        eyev = normalize(eyev);
        float3 refv = reflect(Vp, normal.xyz);
        refv = normalize(refv);
        
        specular = -dot(eyev, refv);
        specular = saturate(specular);
        specular = pow(specular, 30.0f);
        
        float dist = length(In.WorldPosition.xyz - Light.Position.xyz);
        float ofs = saturate(1.0f - dist / Light.PointLightparam.x);
        spot *= ofs;

    }
    
    
    outDiffuse = g_Texture.Sample(g_Sampler, In.TexCoord);
    outDiffuse.rgb *= Light.Diffuse.rgb * In.Diffuse.rgb * light * spot + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;
    outDiffuse.rgb += specular * spot;

}
