#pragma once

#include "DirectXContext.h"

namespace Engine
{
    class DirectXCommandObjects
    {
    public:
        DirectXCommandObjects();
        ~DirectXCommandObjects();

        void Flush();

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

        Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
        UINT64 m_CurrentFence = 0;

        friend class DirectXApi;
        friend class DirectXSwapchain;
    };
    
}
