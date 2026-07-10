/*==============================================================================

[Wolf.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "Wolf.h"
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
//カメラは分離すると駄目なので外す

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Wolf::Init(void)
{

	//シェーダー読み込み
	//CreateVertexShader(&VertexShader, &VertexLayout, "LimLightingVS.cso");
	//CreatePixelShader(&PixelShader, "LimLightingPS.cso");
	CreateVertexShader(&VertexShader, &VertexLayout, "Toon1VS.cso");
	CreatePixelShader(&PixelShader, "Toon1PS.cso");




	//3Dオブジェクト管理構造体の初期化
	//Position = XMFLOAT3(0.0f, 0.4f, 0.5f); // 台座の上

	Position = XMFLOAT3(0.0f + 0.5f * 0.5f, 0.6f, 0.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(0.32f, 0.32f, 0.32f);


	//モデル読み込み
	Model = ModelLoad("asset\\model\\Wolf.fbx");

	// ライト構造体の初期化
	XMVECTOR dir = XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f);
	dir = XMVector3Normalize(dir);
	// 光のベクトル
	XMStoreFloat4(&light.Direction, dir);
	light.Position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	// 光の色
	light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	// 環境光
	light.Ambient = XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f);
	light.PointLightParam = XMFLOAT4(3.0f, 3.0f, 0.0f, 1.0f); // x=光の到達距離, y=リム指数(0だとモデル全体が真っ白になる)


	Parameter = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	Parameter.x = 0.4f;
	Parameter.y = 0.7f;


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Wolf::Finalize(void)
{
	//使ったものを解放

	VertexLayout->Release();
	VertexShader->Release();
	PixelShader->Release();


	ModelRelease(Model);

}

//=============================================================================
// 更新処理
//=============================================================================
void Wolf::Update(void)
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


	ImGui::Begin("Wolf");
	{
		//// 明るさ・リムの強さ・光源位置を調整するスライダー
		//ImGui::SliderFloat3("Diffuse (direct light)", &light.Diffuse.x, 0.0f, 2.0f, "%.2f");
		//ImGui::SliderFloat3("Ambient (base bright)",  &light.Ambient.x, 0.0f, 1.0f, "%.2f");
		//ImGui::SliderFloat("Rim Power (PLP.y)",       &light.PointLightParam.y, 0.5f, 8.0f, "%.2f");
		//ImGui::SliderFloat("Light Reach (PLP.x)",     &light.PointLightParam.x, 0.5f, 5.0f, "%.2f");

		//ImGui::SliderFloat("Position.x", &light.Position.x, -2.0f, 2.0f, "%.2f");
		//ImGui::SliderFloat("Position.y", &light.Position.y, -2.0f, 2.0f, "%.2f");
		//ImGui::SliderFloat("Position.z", &light.Position.z, -2.0f, 2.0f, "%.2f");

		ImGui::SliderFloat("Level-1", &Parameter.x, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Level-2", &Parameter.y, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Edge", &Parameter.z, 0.0f, -0.6f, "%.2f");
	}
	ImGui::End();

}

//=============================================================================
// 描画処理
//=============================================================================
void Wolf::Draw(void)
{
	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	SetLight(light);
	SetParameter(Parameter);
	{//3Dポリゴン1枚の処理
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
		//ワールド行列作成 かけるZの順番に注意
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
