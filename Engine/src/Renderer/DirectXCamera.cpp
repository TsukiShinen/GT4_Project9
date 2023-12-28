#include "DirectXCamera.h"

namespace Engine
{
    DirectXCamera::DirectXCamera(float width, float height, float fovDegree, float nearZ, float farZ)
        : m_FovDegree(fovDegree), m_NearZ(nearZ), m_FarZ(farZ)
    {
        m_transform = std::make_unique<Transform>(DirectX::XMFLOAT3(0.f, 0.f, -2.f));

        //DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(m_transform->GetPosition(), m_transform->GetForwardVector(), m_transform->GetUpVector());
        
        DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse(nullptr,m_transform->GetWorld());

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
        //const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(m_transform->GetPosition(), DirectX::XMVectorAdd(m_transform->GetPosition(),m_transform->GetForwardVector()), m_transform->GetUpVector());
        DirectX::XMMATRIX view = DirectX::XMMatrixInverse(nullptr, m_transform->GetWorld());

        const DirectX::XMMATRIX proj = XMLoadFloat4x4(&m_Proj);

        const DirectX::XMMATRIX viewProj = XMMatrixMultiply(view, proj);

        XMStoreFloat4x4(&m_MainPassCB.ViewProj, XMMatrixTranspose(viewProj));
    }

    void DirectXCamera::GameUpdate(float dt)
    {
        if (Input::IsKeyPressed(Engine::Key::Z)) {
            MoveZAxis(dt, 1.f);
        }
        if (Input::IsKeyPressed(Engine::Key::S)) {
            MoveZAxis(dt, -1.f);
        }
        if (Input::IsKeyPressed(Engine::Key::Q)) {
            MoveXAxis(dt, -1.f);
        }
        if (Input::IsKeyPressed(Engine::Key::D)) {
            MoveXAxis(dt, 1.f);
        }
        if (Input::IsKeyPressed(Engine::Key::A)) {
            Rotate(dt, -.5f);
        }
        if (Input::IsKeyPressed(Engine::Key::E)) {
            Rotate(dt, .5f);
        }
        if (Input::IsKeyPressed(Engine::Key::W)) {
            Pitch(dt, .5f);
        }
        if (Input::IsKeyPressed(Engine::Key::X)) {
            Pitch(dt, -.5f);
        }

    }

    void DirectXCamera::MoveZAxis(float dt, float value)
    {
        DirectX::XMVECTOR forward = m_transform->GetForwardVector();
        DirectX::XMVECTOR s = DirectX::XMVectorReplicate(dt * value);
        DirectX::XMVECTOR position = m_transform->GetPosition();
        DirectX::XMVECTOR newPos = DirectX::XMVectorMultiplyAdd(s, forward, position);

        DirectX::XMFLOAT3 p; 

        DirectX::XMStoreFloat3(&p, newPos);

        m_transform->SetPosition(p);
    }

    void DirectXCamera::MoveXAxis(float dt, float value)
    {
        DirectX::XMVECTOR right = m_transform->GetRightVector();
        DirectX::XMVECTOR s = DirectX::XMVectorReplicate(dt * value);
        DirectX::XMVECTOR position = m_transform->GetPosition();
        DirectX::XMVECTOR newPos = DirectX::XMVectorMultiplyAdd(s, right, position);

        DirectX::XMFLOAT3 p;

        DirectX::XMStoreFloat3(&p, newPos);

        m_transform->SetPosition(p);
    }

    void DirectXCamera::Pitch(float dt, float value)
    {
        m_transform->Rotate(0.f, dt * value , 0.f);
    }

    void DirectXCamera::Rotate(float dt, float value)
    {
        m_transform->Rotate(dt * value, 0.f, 0.f);
    }

}
