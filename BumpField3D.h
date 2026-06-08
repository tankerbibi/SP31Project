#pragma once

#include "main.h"
#include "renderer.h"

class BumpField3D
{
protected:
	XMFLOAT3 Position;
	XMFLOAT3 Scale;
	XMFLOAT3 Rotate;
	int TexID;
	int BumpTexID;

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* VertexLayout;

	LIGHT Light;

public:
	HRESULT Init(void);
	void Finalize(void);
	void Update(void);
	void Draw(void);
};
