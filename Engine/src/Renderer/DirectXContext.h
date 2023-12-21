#pragma once

#include "d3dUtil.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Engine
{
    class DirectXCommandObject;
    class DirectXSwapchain;
    class DirectXPipeline;

    class DirectXContext
    {
    public:
        
        static void Initialize();
        static void Shutdown();

        static DirectXContext* Get() { return s_Instance; }
    private:
        void InitializeMsaa();
        
        Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
        std::unique_ptr<DirectXSwapchain> m_Swapchain;
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;

        std::unique_ptr<DirectXCommandObject> m_CommandObject;

        std::unique_ptr<DirectXPipeline> m_BasePipeline;
        UINT m_CbvSrvUavDescriptorSize = 0;
        
        bool m_4xMsaaState = false;
        UINT m_4xMsaaQuality = 0;
        
        D3D_DRIVER_TYPE m_DriverType = D3D_DRIVER_TYPE_HARDWARE;

    private:
        static DirectXContext* s_Instance;

        friend class DirectXApi;
        friend class DirectXSwapchain;
        friend class DirectXCommandObject;
        friend class DirectXPipeline;
    };
    
}