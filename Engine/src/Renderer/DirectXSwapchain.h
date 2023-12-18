#pragma once

#include "DirectXContext.h"

namespace Engine
{
    class DirectXSwapchain
    {
    public:
        static const int k_SwapChainBufferCount = 2;
        
        static const DXGI_FORMAT k_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        static const DXGI_FORMAT k_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        
        DirectXSwapchain(uint32_t pWidth, uint32_t pHeight);
        ~DirectXSwapchain();

        void Resize(uint32_t pWidth, uint32_t pHeight);

        void Present();

    private:
        ID3D12Resource* CurrentBackBuffer()const;
        D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
        
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_Swapchain;
        
        Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapchainBuffers[k_SwapChainBufferCount];
        Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;
        int m_CurrentBackBuffer = 0;

        D3D12_VIEWPORT m_ScreenViewport; 
        D3D12_RECT m_ScissorRect;

        friend class DirectXApi;
    };
    
}
