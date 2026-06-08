/*==============================================================================

[VertexDirectionalLighting.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "VertexDirectionalLighting.h"
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
HRESULT VertexDirectionalLighting::Init(void)
{

	//シェーダー読み込み
	CreateVertexShader(&VertexShader, &VertexLayout, "VertexDirectionalLightingVS.cso");
	CreatePixelShader(&PixelShader, "VertexDirectionalLightingPS.cso");




	//3Dオブジェクト管理構造体の初期化
	Position = XMFLOAT3(0.5f, 0.2f, 0.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);


	//モデル読み込み
	Model = ModelLoad("asset\\model\\model.fbx");

	// ライト構造体の初期化
	XMVECTOR dir = XMVectorSet(1.0f, -1.0f, 0.5f, 0.0f);
	dir = XMVector3Normalize(dir);
	// 光のベクトル
	XMStoreFloat4(&light.Direction, dir);
	// 光の色
	light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void VertexDirectionalLighting::Finalize(void)
{
	//作ったものを解放

	VertexLayout->Release();
	VertexShader->Release();
	PixelShader->Release();


	ModelRelease(Model);

}

//=============================================================================
// 更新処理
//=============================================================================
void VertexDirectionalLighting::Update(void)
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




}

//=============================================================================
// 描画処理
//=============================================================================
void VertexDirectionalLighting::Draw(void)
{
	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	SetLight(light);

	{//3Dポリゴン１つずつの処理
		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//平行移動行列作成
		XMMATRIX	TranslationMatrix =
			XMMatrixTranslation(
				Position.x,
				Position.y,
				Position.z
			);
		//回転行列作成
		XMMATRIX	RotationMatrix =
			XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(Rotate.x),
				XMConvertToRadians(Rotate.y),
				XMConvertToRadians(Rotate.z)
			);
		//スケーリング行列作成
		XMMATRIX	ScalingMatrix =
			XMMatrixScaling(
				Scale.x,
				Scale.y,
				Scale.z
			);
		//ワールド行列作成 ※乗算の順番に注意
		XMMATRIX	WorldMatrix =
			ScalingMatrix *
			RotationMatrix *
			TranslationMatrix;
		//DirectXへセット
		SetWorldMatrix(WorldMatrix);

		//プリミティブトポロジーの設定
		GetDeviceContext()->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);
		//マテリアル設定
		MATERIAL	material;
		ZeroMemory(&material, sizeof(MATERIAL));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//描画
		ModelDraw(Model);
	}


}
