#include "DirectXCommandObject.h"


namespace Engine
{
    DirectXCommandObject::DirectXCommandObject()
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        THROW_IF_FAILED(DirectXContext::Get()->m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

        THROW_IF_FAILED(DirectXContext::Get()->m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));

        THROW_IF_FAILED(DirectXContext::Get()->m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));

        m_CommandList->Close();

        THROW_IF_FAILED(DirectXContext::Get()->m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&m_Fence)));
    }

    void DirectXCommandObject::Execute()
    {
        THROW_IF_FAILED(m_CommandList->Close());
        ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
        m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    }

    void DirectXCommandObject::Flush()
    {
        m_CurrentFence++;

        THROW_IF_FAILED(m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence));

        if(m_Fence->GetCompletedValue() < m_CurrentFence)
        {
            const HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

            THROW_IF_FAILED(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    HRESULT DirectXCommandObject::ResetList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& pAllocator) const
    {
        return m_CommandList->Reset(pAllocator.Get(), nullptr);
    }
}
