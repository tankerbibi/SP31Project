// CookTorranceSub.hlsl

// マイクロファセット分布関数
float CalculateBeckmann(float smooth, float nh)
{
    // ゼロディバイド対策
    if (nh <= 0.0) 
        return 0.0;
    if (smooth <= 0.0)
        return 0.0;
    
    float smo2 = smooth * smooth;
    float nh2 = nh * nh;
    float nh4 = nh2 * nh2;
    
    float ex = -(1.0f - nh2) / (smo2 * nh2);
    
    float PI = 3.14159265;
    float bun = PI * smo2 * nh4;
    
    return exp(ex) / bun;
}

// フレネル反射光
float CalculateFresnel (float metal, float hv)
{
    metal = lerp(0.04, 1.0, metal);
    return metal + (1.0 - metal) * pow(1.0 - hv, 5.0);
}

// G 幾何減衰
float CalculateGeometricDamping(float nh, float nv, float nl, float vh)
{
    float G;
    vh = max(vh, 0.0001);
    G = min(1.0, min(2.0 * nh * nv / vh, 2.0 * nh * nl / vh));
    // 0未満にしない
    G = max(0.0, G);
    
    return G;
}
