#include "DirectXCommandObject.h"


namespace Engine
{
    DirectXCommandObject::DirectXCommandObject()
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));

        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));

        m_CommandList->Close();

        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&m_Fence)));
    }

    void DirectXCommandObject::Execute()
    {
        ThrowIfFailed(m_CommandList->Close());
        ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
        m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    }

    void DirectXCommandObject::Flush()
    {
        m_CurrentFence++;

        ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence));

        if(m_Fence->GetCompletedValue() < m_CurrentFence)
        {
            const HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

            ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    HRESULT DirectXCommandObject::ResetList() const
    {
        return m_CommandList->Reset(DirectXContext::Get()->m_CommandObject->m_CommandAllocator.Get(), nullptr);
    }

    HRESULT DirectXCommandObject::ResetAllocator() const
    {
        return m_CommandAllocator->Reset();
    }
}
