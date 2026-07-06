/*==============================================================================

[Too1.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "Toon3.h"
#include "renderer.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"
#include "model.h"
#include "keyboard.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
//カメラは複数あるとダメなので外す

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Toon3::Init(void)
{

	//シェーダー読み込み
	CreateVertexShader(&VertexShader, &VertexLayout, "Toon2VS.cso");
	CreatePixelShader(&PixelShader, "Toon2PS.cso");
	CreateVertexShader(&VertexEdgeShader, &VertexEdgeLayout, "ToonVSEdge.cso");
	CreatePixelShader(&PixelEdgeShader, "Toon2PSEdge.cso");


	

	//3Dオブジェクト管理構造体の初期化
	Position = XMFLOAT3(0.0f + 0.5f * 0, 0.3f, 0.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);


	//モデル読み込み
	Model = ModelLoad("asset\\model\\model.fbx");

	XMVECTOR dir = XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f);
	dir = XMVector3Normalize(dir);
	XMStoreFloat4(&Light.Direction, dir);

	Light.Position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	// 光の色
	Light.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	// 環境光
	Light.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	Light.PointLightParam = XMFLOAT4(3.0f, 0.0f, 0.0f, 1.0f);

	Parameter = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	Parameter.x = 0.5f;

	TexID = TextureLoad(L"asset\\texture\\Toon2.png");

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Toon3::Finalize(void)
{
	//作ったものを解放

	VertexLayout->Release();
	VertexShader->Release();
	PixelShader->Release();

	VertexEdgeLayout->Release();
	VertexEdgeShader->Release();
	PixelEdgeShader->Release();

	ModelRelease(Model);

}

//=============================================================================
// 更新処理
//=============================================================================
void Toon3::Update(void)
{
	if (Keyboard_IsKeyDown(KK_UP))
	{
		Position.z += 0.3f * (1.0f / 60.0f);
	}
	else if (Keyboard_IsKeyDown(KK_DOWN))
	{
		Position.z -= 0.3f * (1.0f / 60.0f);
	}
	if (Keyboard_IsKeyDown(KK_RIGHT))
	{
		Position.x += 0.3f * (1.0f / 60.0f);
	}
	else if (Keyboard_IsKeyDown(KK_LEFT))
	{
		Position.x -= 0.3f * (1.0f / 60.0f);
	}
	if (Keyboard_IsKeyDown(KK_Z))
	{
		Rotate.x += 60.0f * (1.0f / 60.0f);
	}
	else if (Keyboard_IsKeyDown(KK_X))
	{
		Rotate.x -= 60.0f * (1.0f / 60.0f);
	}


	ImGui::Begin("Toon3");
	{
		ImGui::SliderFloat("Texture V", &Parameter.x, 0.0f, 1.0f, "%.4f");

		ImGui::SliderFloat("Edge", &Parameter.z, 0.0f, 0.3f, "%.4f");
	}
	ImGui::End();
	//Parameter.y += 0.006f;

}

//=============================================================================
// 描画処理
//=============================================================================
void Toon3::Draw(void)
{
	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	SetLight(Light);
	SetParameter(Parameter);

	{//3Dポリゴン１つずつの処理
		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(1, 1, &tex);

		//平行移動行列作成
		XMMATRIX TranslationMatrix =
			XMMatrixTranslation(
				Position.x,
				Position.y,
				Position.z
			);
		//回転行列作成
		XMMATRIX RotationMatrix =
			XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(Rotate.x),
				XMConvertToRadians(Rotate.y),
				XMConvertToRadians(Rotate.z)
			);
		//スケーリング行列作成
		XMMATRIX ScalingMatrix =
			XMMatrixScaling(
				Scale.x,
				Scale.y,
				Scale.z
			);
		XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;
		SetWorldMatrix(WorldMatrix);
		//プリミティブトポロジーの設定
		GetDeviceContext()->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);

		//描画
		ModelDraw(Model);
	}

	// ここからエッジモデルの描画
	
	// エッジ用シェーダーのセット

	// カリングの切り替え
	
	// エッジモデルの描画

	// カリングの切り替え
}
