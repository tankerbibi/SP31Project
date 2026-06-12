#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
#include "Sprite.h"
#include "Game.h"
#include "keyboard.h"
#include "Camera.h"
#include "texture.h"
#include "field3D.h"

#include "Sprite2D.h"
#include "PolygonModel.h"
#include "VertexDirectionalLighting.h"
#include "PixelDirectionalLighting.h"
#include "PixelLightingBlinnPhong.h"
#include "HemiSphereLighting.h"
#include "PointPixelLighting.h"
#include "LimLighting.h"
#include "SpotLighting.h"
#include "BumpField3D.h"

//===============================================
//グローバル変数
 
Camera		CameraObject;
Sprite2D	Test2d;

Field3D Field;

PointPixelLighting PPL;
LimLighting LL;
Spotlighting SL;
BumpField3D BumpField;

static LIGHT Light;


//ポーズフラグ
static	bool	pause = false;

//===============================================
//ポーズフラグセット
void	SetPause(bool flg)
{
	pause = flg;
}
//===============================================
//ポーズフラグ取得
bool	GetPause()
{
	return pause;
}

//===============================================
//ゲームシーン初期化
void InitGame()
{
	TextureInitialize(GetDevice());
	InitCamera();

	Test2d.Init();

	Field.Init();

	PPL.Init();
	LL.Init();
	SL.Init();
	BumpField.Init();

	// ライト構造体の初期化
	XMVECTOR dir =XMVector3Normalize(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
	// 光のベクトル
	XMStoreFloat4(&Light.Direction, dir);
	Light.Position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	// 光の色
	Light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	// 環境光
	Light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	// x距離 yPow
	Light.PointLightParam = XMFLOAT4(10.0f, 1.0f, 0.0f, 0.0f);
	// コーンの角度
	Light.Angle.x = XMConvertToRadians(30.0f);

	//light.SkyColor = XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f);  // 赤っぽい
	//light.GroundColor = XMFLOAT4(0.0f, 0.6f, 0.0f, 1.0f);  // 緑っぽい
}

//===============================================
//ゲームシーン終了
void FinalizeGame()
{
	FinalizeCamera();
	Test2d.Finalize();

	Field.Finalize();
	PPL.Finalize();
	LL.Finalize();
	SL.Finalize();
	BumpField.Finalize();
	TextureFinalize();
}

//===============================================
//ゲームシーン更新
void UpdateGame()
{

	if (GetPause() == false)//ポーズ中でなければ更新実行
	{
		UpdateCamera();
		Test2d.Update();

		Field.Update();
		PPL.Update();
		LL.Update();
		SL.Update();
		BumpField.Update();
	}

	ImGui::Begin("Spotlighting");
	{
		ImGui::SliderFloat("Position.x", &Light.Position.x, -2.0f, 2.0f, "%.2f");
		ImGui::SliderFloat("Position.y", &Light.Position.y, -2.0f, 2.0f, "%.2f");
		ImGui::SliderFloat("Position.z", &Light.Position.z, -2.0f, 2.0f, "%.2f");

		ImGui::SliderFloat("DiffuseR", &Light.Diffuse.x, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat("DiffuseG", &Light.Diffuse.y, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat("DiffuseB", &Light.Diffuse.z, 0.0f, 1.0f, "%.1f");

		float angle = XMConvertToDegrees(Light.Angle.x);
		ImGui::SliderFloat("ConeAngle", &angle, 5.0f, 45.0f, "%.1f");
		Light.Angle.x = XMConvertToRadians(angle);

		// 距離
		ImGui::SliderFloat("Attenuation", &Light.PointLightParam.x, 0.0f, 10.0f, "%.1f");
		// 調整用
		ImGui::SliderFloat("Pow", &Light.PointLightParam.y, 1.0f, 50.0f, "%.2f");
	}
	ImGui::End();

}

//===============================================
//ゲームシーン描画
void DrawGame()
{

	// 2D用マトリクス設定
	{
		SetWorldViewProjection2D();
		Test2d.Draw();
	}

	// 3D用マトリクス設定
	SetDepthEnable(true); //  奥行き処理有効
	DrawCamera();
	{   // 同じライトで表示
		
		SetLight(Light);
		Field.Draw();
		SL.Draw();
	}
	{  // 個別のライトで表示
		//BumpField.Draw();
		/*PPL.Draw();
		LL.Draw();*/
	}
}
