#include "DirectXCamera.h"

namespace Engine
{
    DirectXCamera::DirectXCamera(float width, float height, float fovDegree, float nearZ, float farZ)
        : m_FovDegree(fovDegree), m_NearZ(nearZ), m_FarZ(farZ)
    {
        m_Transform = std::make_unique<Transform>(DirectX::XMFLOAT3(0.f, 0.f, -2.f));

        DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse(nullptr, m_Transform->GetWorld());

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
        DirectX::XMMATRIX view = DirectX::XMMatrixInverse(nullptr, m_Transform->GetWorld());

        const DirectX::XMMATRIX proj = XMLoadFloat4x4(&m_Proj);

        const DirectX::XMMATRIX viewProj = XMMatrixMultiply(view, proj);

        XMStoreFloat4x4(&m_ViewProj, viewProj);
        XMStoreFloat4x4(&m_ViewProjT, XMMatrixTranspose(viewProj));
    }

    void DirectXCamera::GameUpdate(float dt)
    {
        if (Input::IsKeyPressed(Engine::Key::Z)) {
            m_Transform->MoveForward(dt * 5.f, 1.f);
        }
        if (Input::IsKeyPressed(Engine::Key::S)) {
            m_Transform->MoveForward(dt * 5.f, -1.f);
        }
        if (Input::IsKeyPressed(Engine::Key::Q)) {
            m_Transform->MoveRight(dt * 5.f, -1.f);
        }
        if (Input::IsKeyPressed(Engine::Key::D)) {
            m_Transform->MoveRight(dt * 5.f, 1.f);
        }
        if (Input::IsKeyPressed(Engine::Key::A)) {
            m_Transform->MoveUp(dt * 5.f, 1.f);
        }
        if (Input::IsKeyPressed(Engine::Key::E)) {
            m_Transform->MoveUp(dt * 5.f, -1.f);
        }
    }

    void DirectXCamera::MouseMove(float x, float y)
    {
        if (Input::IsMouseButtonPressed(Engine::Mouse::Button1)) {
            float dx = DirectX::XMConvertToRadians(0.25f * (x - m_LastMousePos.x));
            float dy = DirectX::XMConvertToRadians(0.25f * (y - m_LastMousePos.y));

            m_Transform->RotateWorldY(dx);
            m_Transform->RotateLocalX(dy);
        }
        m_LastMousePos = { x,y };
    }
}
