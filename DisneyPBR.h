/*==============================================================================

   [DisneyPBR.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once


#include "main.h"
#include "renderer.h"
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


////////////////
//構造体
////////////////


class DisneyPBR
{
protected:
	XMFLOAT3	Position;
	XMFLOAT3	Scale;
	XMFLOAT3	Rotate;
	int			TexID;

	int TexIDRoughness;
	int TexIDMetalness;

	ID3D11VertexShader* VertexShader; //頂点シェーダーオブジェクト
	ID3D11PixelShader* PixelShader; //ピクセルシェーダーオブジェクト
	ID3D11InputLayout* VertexLayout; //頂点レイアウトオブジェクト

	LIGHT light;
	XMFLOAT4 Parameter;


	MODEL* Model;

public:

	HRESULT Init(void);
	void Finalize(void);
	void Update(void);
	void Draw(void);



};







//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

