#include "Building.h"
#include "Camera.h"
#include "texture.h"
#include "model.h"

struct CubeInstance
{
    XMFLOAT3 Position;
    XMFLOAT3 Scale;
};

// ---- ビル列の設定：ここの数値だけで列の長さ・間隔・位置を調整できる ----
// 片側 BUILDING_COUNT 棟を手前(-Z)から奥(+Z)へ並べる。
// Position.y = 高さ/2 にすることで各ビルが床(Y=0)に接地する。
static const int   BUILDING_COUNT   = 14;     // 片側の棟数（増やすほど列が長くなる）
static const float BUILDING_START_Z = -1.3f;  // 一番手前のビルのZ
static const float BUILDING_SPACING = 1.0f;   // 奥方向のビル間隔
static const float BUILDING_SIDE_X  = 1.4f;   // 中央から左右の列までの距離
static const float BUILDING_WIDTH   = 0.50f;  // ビルの幅・奥行き

// 高さのパターン（列方向に繰り返して街並みの起伏を出す）
static const float LeftHeights[]  = { 1.1f, 1.7f, 1.3f, 2.0f, 1.5f, 1.0f, 1.8f };
static const float RightHeights[] = { 1.5f, 1.1f, 1.9f, 1.3f, 1.7f, 1.2f, 2.0f };

static void DrawCube(MODEL* model, const CubeInstance& inst)
{
    XMMATRIX world =
        XMMatrixScaling(inst.Scale.x, inst.Scale.y, inst.Scale.z) *
        XMMatrixTranslation(inst.Position.x, inst.Position.y, inst.Position.z);
    SetWorldMatrix(world);

    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    MATERIAL mat;
    ZeroMemory(&mat, sizeof(mat));
    mat.Diffuse = XMFLOAT4(0.75f, 0.70f, 0.65f, 1.0f);
    SetMaterial(mat);

    ModelDraw(model);
}

// ビルの列を1列(+Z方向)に描画する。heights[]を繰り返して高さに変化を付ける。
static void DrawBuildingRow(MODEL* model, float sideX, const float* heights, int patternLen)
{
    for (int i = 0; i < BUILDING_COUNT; i++)
    {
        float h = heights[i % patternLen];
        CubeInstance inst;
        inst.Position = XMFLOAT3(sideX, h * 0.5f, BUILDING_START_Z + i * BUILDING_SPACING);
        inst.Scale    = XMFLOAT3(BUILDING_WIDTH, h, BUILDING_WIDTH);
        DrawCube(model, inst);
    }
}

HRESULT Building::Init(void)
{
    CreateVertexShader(&VertexShader, &VertexLayout, "PointPixelLightingVS.cso");
    CreatePixelShader(&PixelShader, "PointPixelLightingPS.cso");

    Model = ModelLoad("asset\\model\\cube.fbx");

    XMVECTOR dir = XMVector3Normalize(XMVectorSet(0.3f, -1.0f, 0.5f, 0.0f));
    XMStoreFloat4(&light.Direction, dir);
    light.Position        = XMFLOAT4(0.0f, 2.0f, -1.0f, 1.0f);
    light.Diffuse         = XMFLOAT4(0.9f, 0.85f, 0.8f, 1.0f);
    light.Ambient         = XMFLOAT4(0.35f, 0.35f, 0.35f, 1.0f);
    light.PointLightParam = XMFLOAT4(30.0f, 2.0f, 0.0f, 1.0f); // x=光の到達距離：長い列の奥まで届くよう広めにする

    return S_OK;
}

void Building::Finalize(void)
{
    VertexLayout->Release();
    VertexShader->Release();
    PixelShader->Release();
    ModelRelease(Model);
}

void Building::Update(void)
{
    ImGui::Begin("Building Light");
    {
        ImGui::SliderFloat("Pos X", &light.Position.x, -3.0f, 3.0f, "%.2f");
        ImGui::SliderFloat("Pos Y", &light.Position.y, -3.0f, 5.0f, "%.2f");
        ImGui::SliderFloat("Pos Z", &light.Position.z, -3.0f, 3.0f, "%.2f");
    }
    ImGui::End();
}

void Building::Draw(void)
{
    GetDeviceContext()->IASetInputLayout(VertexLayout);
    GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
    GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);
    SetLight(light);

    // 左右の列を描画（中央から ±BUILDING_SIDE_X の位置に並べる）
    DrawBuildingRow(Model, -BUILDING_SIDE_X, LeftHeights,  _countof(LeftHeights));
    DrawBuildingRow(Model,  BUILDING_SIDE_X, RightHeights, _countof(RightHeights));
}
