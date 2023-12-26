#pragma once

#include "d3dUtil.h"
#include "DirectXFrameData.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Engine
{
    class DirectXCommandObject;
    class DirectXSwapchain;
    class Shader;

    class DirectXContext
    {
    public:
        
        static void Initialize();
        static void Shutdown();

        static DirectXContext* Get() { return s_Instance; }
    private:
        void InitializeMsaa();
        DirectXFrameData& CurrentFrameData() const { return *m_FramesData[m_CurrentFrameData]; }
        
        Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
        std::unique_ptr<DirectXSwapchain> m_Swapchain;
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;

        std::unique_ptr<DirectXCommandObject> m_CommandObject;

        std::unique_ptr<Shader> m_BasePipeline;
        
        UINT m_CbvSrvUavDescriptorSize = 0;
        
        bool m_4xMsaaState = false;
        UINT m_4xMsaaQuality = 0;
        
        D3D_DRIVER_TYPE m_DriverType = D3D_DRIVER_TYPE_HARDWARE;
        
        std::vector<std::unique_ptr<DirectXFrameData>> m_FramesData;
        int m_CurrentFrameData = 0;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_PassConstantHeap = nullptr;

        // Camera
        PassConstants m_MainPassCB;
        DirectX::XMFLOAT3 m_EyePosition = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT4X4 m_View = MathHelper::Identity4x4();
        DirectX::XMFLOAT4X4 m_Proj = MathHelper::Identity4x4();

    private:
        static DirectXContext* s_Instance;

        friend class DirectXApi;
        friend class DirectXSwapchain;
        friend class DirectXCommandObject;
        friend class Shader;
        friend class Mesh;
    };
    
}