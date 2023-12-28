#include "DirectXCamera.h"

namespace Engine
{
    DirectXCamera::DirectXCamera(float width, float height, float fovDegree, float nearZ, float farZ)
        : m_FovDegree(fovDegree), m_NearZ(nearZ), m_FarZ(farZ)
    {
        DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(0.f, 0.f, -5.f, 0.f);
        DirectX::XMVECTOR lookAtPos = DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f);
        DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);

        DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(eyePos, lookAtPos, upVector);
        XMStoreFloat4x4(&m_View, viewMatrix);

        Resize(width, height);
    }

    DirectXCamera::~DirectXCamera()
    {
    }

    void DirectXCamera::Resize(float width, float height)
    {
        float fovRad = (m_FovDegree / 360.f) * DirectX::XM_2PI;
        float aspectRatio = width / height;
        DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, m_NearZ, m_FarZ);
        XMStoreFloat4x4(&m_Proj, projMatrix);
    }

    void DirectXCamera::Update()
    {
        const DirectX::XMMATRIX view = XMLoadFloat4x4(&m_View);
        const DirectX::XMMATRIX proj = XMLoadFloat4x4(&m_Proj);

        const DirectX::XMMATRIX viewProj = XMMatrixMultiply(view, proj);

        XMStoreFloat4x4(&m_ViewProj, viewProj);
        XMStoreFloat4x4(&m_ViewProjT, XMMatrixTranspose(viewProj));
    }

}
