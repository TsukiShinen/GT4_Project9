#pragma once

#include "d3dUtil.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Engine
{
    class DirectXCommandObjects;
    class DirectXSwapchain;
    class DirectXPipeline;

    class DirectXContext
    {
    public:
        
        static void Initialize();
        static void Shutdown();

        static DirectXContext* Get() { return s_Instance; }
    private:
        void CreateRtvAndDsvDescriptorHeaps();

        void InitializeMsaa();
        
        Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
        std::unique_ptr<DirectXSwapchain> m_Swapchain;
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;

        std::unique_ptr<DirectXCommandObjects> m_CommandObjects;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

        std::unique_ptr<DirectXPipeline> m_BasePipeline;

        UINT m_RtvDescriptorSize = 0;
        UINT m_DsvDescriptorSize = 0;
        UINT m_CbvSrvUavDescriptorSize = 0;
        
        bool m_4xMsaaState = false;
        UINT m_4xMsaaQuality = 0;
        
        D3D_DRIVER_TYPE m_DriverType = D3D_DRIVER_TYPE_HARDWARE;

    private:
        static DirectXContext* s_Instance;

        friend class DirectXApi;
        friend class DirectXSwapchain;
        friend class DirectXCommandObjects;
        friend class DirectXPipeline;
    };
    
}