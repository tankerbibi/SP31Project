#pragma once

#include "main.h"
#include "renderer.h"
#include "model.h"

class Building
{
protected:
    ID3D11VertexShader* VertexShader;
    ID3D11PixelShader*  PixelShader;
    ID3D11InputLayout*  VertexLayout;
    LIGHT light;
    MODEL* Model;

public:
    HRESULT Init(void);
    void Finalize(void);
    void Update(void);
    void Draw(void);
};
