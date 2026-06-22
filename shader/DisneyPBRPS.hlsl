#include "Common.hlsl"
#include "CookTorranceSub.hlsl"

Texture2D g_Texture : register(t0);  // テクスチャ0番
Texture2D g_TextureRoughness : register(t1);
Texture2D g_TextureMetalness : register(t2);

SamplerState g_Sampler : register(s0);

float CalculateDiffuseFromFresnel(float3 N, float3 L, float3 V);
float CalculateCookTorranceSpecular(float3 L, float3 V, float3 N, float smooth, float metallic);

static const float PI = 3.1415926f;

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    // 法線を計算
    float4 normal = normalize(In.Normal);
    // 各種マップをサンプリングする
    // アルベドカラー
    float4 albedoColor = g_Texture.Sample(g_Sampler, In.TexCoord);
    // スペキュラーカラーはアルベドカラーと同じにする
    float3 specColor = albedoColor;
    // テクスチャから金属度と滑らかさを取得
    // 色ではないのでR要素の情報を　色からベクトルに変換して使用
    float smooth = g_TextureRoughness.Sample(g_Sampler, In.TexCoord).r * 2.0f - 1.0f;
    // 色の情報として入っているんで、０から１にしています
    smooth = saturate(smooth);
    
    float metallic = g_TextureMetalness.Sample(g_Sampler, In.TexCoord).r * 2.0f - 1.0f;
    // 色の情報として入っているんで、０から１にしています
    metallic = saturate(metallic);
    // カメラへのベクトルを計算する
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);
    // ライトマシマシ
    float3 lit = 0;
    // ライトが複数あると仮定
    for (int ligNo = 0; ligNo < Parameter.z; ligNo++)
    {
        // 光源へのベクトル
        float4 lv = Light.Position - In.WorldPosition;
        // フレネル反射を考慮した拡散反射を計算
        float diffuseFromFresnel = CalculateDiffuseFromFresnel(normal.xyz, lv.xyz, eyev);
        // 正規化Lambert拡散反射を求める
        float nl = saturate(dot(normal.xyz, lv.xyz));
        float3 light = nl + Light.Diffuse.rgb / PI;
        // 最終的な拡散反射光を計算する
        float3 diffuse = albedoColor.rgb * diffuseFromFresnel * Light.Diffuse.rgb * light;
        // 鏡面反射率を計算する
        float3 spec = CalculateCookTorranceSpecular(lv.xyz, eyev, normal.xyz, smooth, metallic) * Light.Diffuse;
        // 金蔵ドが高ければ、鏡面反射はスペキュラーカラー、低ければ白になるように補間
        // スペキュラーカラーの強さを鏡面反射率として扱う
        spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);
        // 滑らかさが高ければ、拡散反射は弱くなる
        lit += diffuse * (1.0f - smooth) + spec;
    }
    // 環境項により底上げ
    lit += Light.Ambient.rgb * albedoColor.rgb;
    // 結果出力
    outDiffuse.rgb = lit;
    outDiffuse.a = albedoColor.a * In.Diffuse.a;
}

float CalculateDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
    // 法線と光源に向かうベクトルがどれだけ似ているかを内積で求める
    float nl = saturate(dot(N, L));
    // 法線と視線に向かうベクトルがどれだけ似ているかを内積で求める
    float nv = saturate(dot(N, V));
    // 法線と光源への方向に依存する拡散反射率と、法線と視点ベクトルに依存する拡散反射率を
    // 乗算して最終的な拡散反射率を求めている。
    return (nl * nv);
}
float CalculateCookTorranceSpecular(float3 L, float3 V, float3 N, float smooth, float metallic)
{
    //　ライトに向かうベきとると視線に向かうベクトルのハーフベクトルを求める
    float3 H = normalize(L + V);
    
    // 各種ベクトルがどれくらい似ている野かを内積を利用して求める
    float nh = saturate(dot(N, H));
    float vh = saturate(dot(V, H));
    float nl = saturate(dot(N, L));
    float nv = saturate(dot(N, V));
    // D項をベックマン分布を用いて計算する
    float D = CalculateBeckmann(smooth, nh);
    // F項をSchlick近似を用いて計算する
    float F = CalculateFresnel(metallic, vh);
    // G項を求める
    float G = CalculateGeometricDamping(nh, nv, nl, vh);
    // m項を求める
    float m = PI * nv * nh;
    // ここまで求めた値を利用してCook-Torranceモデルの鏡面反射を求める
    return max(F * D * G / m, 0.0);
}
