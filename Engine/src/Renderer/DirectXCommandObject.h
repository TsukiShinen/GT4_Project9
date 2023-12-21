#pragma once

#include "DirectXContext.h"

namespace Engine
{
    class DirectXCommandObject
    {
    public:
        DirectXCommandObject();
        ~DirectXCommandObject() = default;

        void Execute();
        void Flush();
        [[nodiscard]] HRESULT ResetList() const;
        [[nodiscard]] HRESULT ResetAllocator() const;

        Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetCommandQueue() { return m_CommandQueue; }
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GetCommandList() { return m_CommandList; }

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

        Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
        UINT64 m_CurrentFence = 0;
    };
    
}
