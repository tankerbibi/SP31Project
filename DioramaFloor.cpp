#include "DioramaFloor.h"
#include "sprite.h"
#include "texture.h"

#define FLOOR_NUM_VERTEX (4)
#define FLOOR_HALF       (14.0f)   // half size; large enough for the long building rows
#define FLOOR_TSIZE      (7.0f)    // kept proportional to FLOOR_HALF (= HALF/2) so tile density is unchanged

static VERTEX_3D FloorVerts[FLOOR_NUM_VERTEX] =
{
    { XMFLOAT3(-FLOOR_HALF, 0.0f,  FLOOR_HALF), XMFLOAT3(0,1,0), XMFLOAT4(1,1,1,1), XMFLOAT2(0,0) },
    { XMFLOAT3( FLOOR_HALF, 0.0f,  FLOOR_HALF), XMFLOAT3(0,1,0), XMFLOAT4(1,1,1,1), XMFLOAT2(FLOOR_TSIZE,0) },
    { XMFLOAT3(-FLOOR_HALF, 0.0f, -FLOOR_HALF), XMFLOAT3(0,1,0), XMFLOAT4(1,1,1,1), XMFLOAT2(0,FLOOR_TSIZE) },
    { XMFLOAT3( FLOOR_HALF, 0.0f, -FLOOR_HALF), XMFLOAT3(0,1,0), XMFLOAT4(1,1,1,1), XMFLOAT2(FLOOR_TSIZE,FLOOR_TSIZE) },
};

HRESULT DioramaFloor::Init(void)
{
    TexID     = TextureLoad(L"asset\\texture\\sura.jpg");
    BumpTexID = TextureLoad(L"asset\\texture\\Normal.png");

    CreateVertexShader(&VertexShader, &VertexLayout, "BumpVS.cso");
    CreatePixelShader(&PixelShader, "BumpPS.cso");

    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage          = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth      = sizeof(VERTEX_3D) * FLOOR_NUM_VERTEX;
        bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
        GetDevice()->CreateBuffer(&bd, NULL, &VertexBuffer);

        D3D11_MAPPED_SUBRESOURCE msr;
        GetDeviceContext()->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
        CopyMemory(msr.pData, FloorVerts, sizeof(VERTEX_3D) * FLOOR_NUM_VERTEX);
        GetDeviceContext()->Unmap(VertexBuffer, 0);
    }

    Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Rotate   = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Scale    = XMFLOAT3(1.0f, 1.0f, 1.0f);

    XMVECTOR dir = XMVector3Normalize(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
    XMStoreFloat4(&Light.Direction, dir);
    Light.Position        = XMFLOAT4(0.0f, 2.0f, 0.0f, 1.0f);
    Light.Diffuse         = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    Light.Ambient         = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    Light.PointLightParam = XMFLOAT4(3000.0f, 0.0f, 0.0f, 1.0f);

    return S_OK;
}

void DioramaFloor::Finalize(void)
{
    VertexLayout->Release();
    VertexShader->Release();
    PixelShader->Release();
    VertexBuffer->Release();
}

void DioramaFloor::Update(void)
{
    ImGui::Begin("DioramaFloor");
    {
        ImGui::SliderFloat("Light X", &Light.Position.x, -5.0f, 5.0f, "%.1f");
        ImGui::SliderFloat("Light Y", &Light.Position.y, -5.0f, 5.0f, "%.1f");
        ImGui::SliderFloat("Light Z", &Light.Position.z, -5.0f, 5.0f, "%.1f");
    }
    ImGui::End();
}

void DioramaFloor::Draw(void)
{
    GetDeviceContext()->IASetInputLayout(VertexLayout);
    GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
    GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

    SetLight(Light);

    ID3D11ShaderResourceView* tex = GetTexture(TexID);
    GetDeviceContext()->PSSetShaderResources(0, 1, &tex);
    tex = GetTexture(BumpTexID);
    GetDeviceContext()->PSSetShaderResources(1, 1, &tex);

    XMMATRIX world =
        XMMatrixScaling(Scale.x, Scale.y, Scale.z) *
        XMMatrixTranslation(Position.x, Position.y, Position.z);
    SetWorldMatrix(world);

    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    MATERIAL mat;
    ZeroMemory(&mat, sizeof(mat));
    mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    SetMaterial(mat);

    GetDeviceContext()->Draw(FLOOR_NUM_VERTEX, 0);
}
