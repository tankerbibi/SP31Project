#pragma once

#include "main.h"
#include "renderer.h"
#include "model.h"

class Pedestal
{
protected:
    XMFLOAT3 Position;
    XMFLOAT3 Scale;
    XMFLOAT3 Rotate;
    int TexIDRoughness;
    int TexIDMetalness;

    ID3D11VertexShader* VertexShader;
    ID3D11PixelShader*  PixelShader;
    ID3D11InputLayout*  VertexLayout;

    XMFLOAT4 Parameter;
    MODEL* Model;

public:
    HRESULT Init(void);
    void Finalize(void);
    void Update(void);
    void Draw(void);
};
