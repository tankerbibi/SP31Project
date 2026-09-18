/*==============================================================================

   頂点管理 [Posterize.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "Posterize.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Posterize::Init(void)
{
	//シェーダー読み込み
	CreateVertexShader(&VertexShader, &VertexLayout, "PosterizeVS.cso");
	CreatePixelShader(&PixelShader, "PosterizePS.cso");


	//2Dオブジェクト初期化
	Position = XMFLOAT3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Scale = XMFLOAT2(1.0f, 1.0f);
	Size = XMFLOAT2(SCREEN_WIDTH, SCREEN_WIDTH);
	Rotate = 0.0f;

	Parameter = XMFLOAT4(64.0f, 0.2f, 0.7f, 1.0f);
	TexID = TextureLoad(L"asset\\texture\\Designer.png");

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Posterize::Finalize(void)
{
	VertexLayout->Release();
	VertexShader->Release();
	PixelShader->Release();

}

//=============================================================================
// 更新処理
//=============================================================================
void Posterize::Update(void)
{
	ImGui::Begin("Posterize");
	{
		ImGui::SliderFloat("Gradation", &Parameter.x, 1.0f, 64.0f, "%.0f");  // １～６４段階
		ImGui::SliderFloat("MIN", &Parameter.y, 0.0f, 1.0f, "%.3f");
		ImGui::SliderFloat("MAX", &Parameter.z, 0.0f, 1.0f, "%.3f");
		ImGui::SliderFloat("POW", &Parameter.w, 1.0f, 30.0f, "%.0f");
	}
	ImGui::End();

}

//=============================================================================
// 描画処理
//=============================================================================
void Posterize::Draw(void)
{

	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	//奥行き処理をOFF
	SetDepthEnable(false);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);
	SetParameter(Parameter);

	{//2Dポリゴン1枚ずつで必要な処理

		//テクスチャをセット
		//ID3D11ShaderResourceView* tex = GetPeTexture(TexID);  // TexIDは書いても書かなくてもどっちでもイイっぽい？
		ID3D11ShaderResourceView* tex = GetTexture(TexID);  // 読み込んだテクスチャで表示
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//平行移動行列の作成（表示座標を決める）
		XMMATRIX	TranslationMatrix = XMMatrixTranslation(
			Position.x, Position.y, 0.0f);

		//回転行列（Z回転）行列の作成
		XMMATRIX	RotationMatrix = XMMatrixRotationZ(XMConvertToRadians(Rotate));

		//スケーリング行列作成（倍率1.0が等倍、0倍はダメ！）
		XMMATRIX	ScalingMatrix = XMMatrixScaling(Scale.x, Scale.y, 1.0f);

		//ワールド行列の作成（ポリゴンの表示の仕方を指定する最終的な行列
		XMMATRIX	WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

		//ワールド行列をDirectXへセット
		SetWorldMatrix(WorldMatrix);

		GetDeviceContext()->GenerateMips(tex);
		// ポリゴン描画
		DrawSprite(Size, Color);
	}


}
