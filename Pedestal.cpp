#include "Pedestal.h"
#include "Camera.h"
#include "texture.h"
#include "model.h"

HRESULT Pedestal::Init(void)
{
    CreateVertexShader(&VertexShader, &VertexLayout, "DisneyPBRVS.cso");
    CreatePixelShader(&PixelShader, "DisneyPBRPS.cso");

    TexIDRoughness = TextureLoad(L"asset\\texture\\Roughness.jpg");
    TexIDMetalness = TextureLoad(L"asset\\texture\\Metalness.jpg");

    Model = ModelLoad("asset\\model\\cube.fbx");

    Position  = XMFLOAT3(0.0f, 0.15f, 0.2f);
    Rotate    = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Scale     = XMFLOAT3(0.6f, 0.3f, 0.6f);
    Parameter = XMFLOAT4(0.4f, 0.6f, 4.0f, 0.0f);

    return S_OK;
}

void Pedestal::Finalize(void)
{
    VertexLayout->Release();
    VertexShader->Release();
    PixelShader->Release();
    ModelRelease(Model);
}

void Pedestal::Update(void)
{
    ImGui::Begin("Pedestal");
    {
        ImGui::SliderFloat("Roughness", &Parameter.x, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Metalness", &Parameter.y, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Light",     &Parameter.z, 1.0f, 15.0f, "%.1f");
    }
    ImGui::End();
}

void Pedestal::Draw(void)
{
    GetDeviceContext()->IASetInputLayout(VertexLayout);
    GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
    GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);
    SetParameter(Parameter);

    ID3D11ShaderResourceView* tex = GetTexture(TexIDRoughness);
    GetDeviceContext()->PSSetShaderResources(1, 1, &tex);
    tex = GetTexture(TexIDMetalness);
    GetDeviceContext()->PSSetShaderResources(2, 1, &tex);

    XMMATRIX world =
        XMMatrixScaling(Scale.x, Scale.y, Scale.z) *
        XMMatrixRotationRollPitchYaw(
            XMConvertToRadians(Rotate.x),
            XMConvertToRadians(Rotate.y),
            XMConvertToRadians(Rotate.z)) *
        XMMatrixTranslation(Position.x, Position.y, Position.z);
    SetWorldMatrix(world);

    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    MATERIAL mat;
    ZeroMemory(&mat, sizeof(mat));
    mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    SetMaterial(mat);

    ModelDraw(Model);
}
