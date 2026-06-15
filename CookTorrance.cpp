/*==============================================================================

[CookTorrance.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "CookTorrance.h"
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
HRESULT CookTorrance::Init(void)
{

	//シェーダー読み込み
	CreateVertexShader(&VertexShader, &VertexLayout, "CookTorranceVS.cso");
	CreatePixelShader(&PixelShader, "CookTorrancePS.cso");

	//3Dオブジェクト管理構造体の初期化
	Position = XMFLOAT3(0.0f + 0.5f * 2, 0.5f, 0.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);


	//モデル読み込み
	Model = ModelLoad("asset\\model\\model.fbx");

	// ライト構造体の初期化
	XMVECTOR dir = XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f);
	dir = XMVector3Normalize(dir);
	// 光のベクトルa
	XMStoreFloat4(&light.Direction, dir);
	light.Position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	// 光の色
	light.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	// 環境光
	light.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	light.PointLightParam = XMFLOAT4(3.0f, 3.0f, 0.0f, 1.0f);

	Parameter = XMFLOAT4(0, 0, 0, 0);
	Parameter.x = 0.3f;
	Parameter.y = 0.8f;


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CookTorrance::Finalize(void)
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
void CookTorrance::Update(void)
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

	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);
	ImGui::Begin("TEST");
	{
		ImGui::SliderFloat("Roughness", &Parameter.x, 0.0f, 1.0f, "%.3f");
		ImGui::SliderFloat("Metallic", &Parameter.y, 0.0f, 1.0f, "%.3f");
	}
	ImGui::End();

	//ImGui::Begin("CookTorrance");
	//{
	//	// PointLightParamとは光の届く距離。
	//	ImGui::SliderFloat("PointLightParam.x", &light.PointLightParam.x, 0.5f, 5.0f, "%.2f");

	//	ImGui::SliderFloat("PointLightParam.y", &light.PointLightParam.y, 1.0f, 5.0f, "%.1f");

	//	ImGui::SliderFloat("Position.x", &light.Position.x, -2.0f, 2.0f, "%.2f");
	//	ImGui::SliderFloat("Position.y", &light.Position.y, -2.0f, 2.0f, "%.2f");
	//	ImGui::SliderFloat("Position.z", &light.Position.z, -2.0f, 2.0f, "%.2f");
	//}
	//ImGui::End();

}

//=============================================================================
// 描画処理
//=============================================================================
void CookTorrance::Draw(void)
{
	// Parameterをシェーダーへ送る
	SetParameter(Parameter);
	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

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
