#include "main.h"
#include "renderer.h"
#include "Manager.h"
#include "sprite.h"
#include "Game.h"
#include "keyboard.h"
#include "Camera.h"
#include "texture.h"

#include "Sprite2D.h"
#include "Wolf.h"
#include "Building.h"
#include "Pedestal.h"
#include "DioramaFloor.h"
#include "Toon1.h"
#include "Toon2.h"
#include "Toon3.h"
#include "Mosaic.h"

Camera       CameraObject;
Sprite2D     Test2d;

Wolf         WolfObj;
Building     BuildingObj;
Pedestal     PedestalObj;
DioramaFloor DioramaFloorObj;
Toon1 toon1;
Toon2 toon2;
Toon3 toon3;
Mosaic MosaicObject;

static LIGHT Light;
static bool  pause = false;

void SetPause(bool flg) { pause = flg; }
bool GetPause()         { return pause; }

void InitGame()
{
    TextureInitialize(GetDevice());
    InitCamera();

    Test2d.Init();
    WolfObj.Init();
    BuildingObj.Init();
    PedestalObj.Init();
    DioramaFloorObj.Init();
    toon1.Init();
    toon2.Init();
    toon3.Init();
	MosaicObject.Init();


    XMVECTOR dir = XMVector4Normalize(XMVectorSet(0.3f, -1.0f, 0.5f, 0.0f));
    XMStoreFloat4(&Light.Direction, dir);
    Light.Position        = XMFLOAT4(0.0f, 2.0f, -1.0f, 0.0f);
    Light.Diffuse         = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
    Light.Ambient         = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    Light.PointLightParam = XMFLOAT4(10.0f, 2.0f, 0.0f, 0.0f);
}

void FinalizeGame()
{
    FinalizeCamera();
    Test2d.Finalize();
    WolfObj.Finalize();
    BuildingObj.Finalize();
    PedestalObj.Finalize();
    DioramaFloorObj.Finalize();
    toon1.Finalize();
    toon2.Finalize();
    toon3.Finalize();
    MosaicObject.Finalize();
    TextureFinalize();
}

void UpdateGame()
{
    if (!GetPause())
    {
        UpdateCamera();
        Test2d.Update();
        WolfObj.Update();
        BuildingObj.Update();
        PedestalObj.Update();
        DioramaFloorObj.Update();
        toon1.Update();
        toon2.Update();
        toon3.Update();
        MosaicObject.Update();
    }

    ImGui::Begin("Global Light");
    {
        ImGui::SliderFloat("Pos X",     &Light.Position.x, -5.0f, 5.0f, "%.2f");
        ImGui::SliderFloat("Pos Y",     &Light.Position.y,  0.0f, 8.0f, "%.2f");
        ImGui::SliderFloat("Pos Z",     &Light.Position.z, -5.0f, 5.0f, "%.2f");
        ImGui::SliderFloat("Diffuse R", &Light.Diffuse.x,   0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Diffuse G", &Light.Diffuse.y,   0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Diffuse B", &Light.Diffuse.z,   0.0f, 1.0f, "%.2f");
    }
    ImGui::End();
}

void DrawGame()
{

    BeginPe();  // レンダリングテクスチャへ描くようにする
    SetDepthEnable(true);
    DrawCamera();
    SetLight(Light);

    DioramaFloorObj.Draw();
    BuildingObj.Draw();
    PedestalObj.Draw();
    WolfObj.Draw();
    toon1.Draw();
    //toon2.Draw();
    toon3.Draw();

    Clear();  // デフォルトのレンダリングターゲットへ戻す
    SetWorldViewProjection2D();  // 2D用マトリクス設定
    Test2d.Draw();
    MosaicObject.Draw();  // 最後にレンダリングテクスチャを描く
}
