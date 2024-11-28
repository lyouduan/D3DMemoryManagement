#include "D3D12RHI.h"
#include "stdafx.h"
#include "DXSample.h"

using namespace Microsoft::WRL;

namespace TD3D12RHI
{
    ID3D12Device* g_Device = nullptr;
    TD3D12CommandContext g_CommandContext;
    ComPtr<IDXGISwapChain1> g_SwapCHain;

    // memory allocator
    std::unique_ptr<TD3D12UploadBufferAllocator> UploadBufferAllocator = nullptr;
    std::unique_ptr<TD3D12DefaultBufferAllocator> DefaultBufferAllocator = nullptr;
    std::unique_ptr<TD3D12TextureResourceAllocator> TextureResourceAllocator = nullptr;
    std::unique_ptr<TD3D12PiexlResourceAllocator> PixelResourceAllocator = nullptr;

    // heapSlot allocator
    std::unique_ptr<TD3D12HeapSlotAllocator> RTVHeapSlotAllocator = nullptr;
    std::unique_ptr<TD3D12HeapSlotAllocator> DSVHeapSlotAllocator = nullptr;
    std::unique_ptr<TD3D12HeapSlotAllocator> SRVHeapSlotAllocator = nullptr;

    // cache descriptor handle
    std::unique_ptr<TD3D12DescriptorCache> DescriptorCache = nullptr;

    void Initialze()
    {
        // initialize CommandContext
        g_CommandContext.CreateCommandContext(g_Device);

        InitialzeAllocator();
    }

    void InitialzeAllocator()
    {
        UploadBufferAllocator = std::make_unique<TD3D12UploadBufferAllocator>(g_Device);
        DefaultBufferAllocator = std::make_unique<TD3D12DefaultBufferAllocator>(g_Device);
        TextureResourceAllocator = std::make_unique<TD3D12TextureResourceAllocator>(g_Device);
        PixelResourceAllocator = std::make_unique<TD3D12PiexlResourceAllocator>(g_Device);

        RTVHeapSlotAllocator = std::make_unique<TD3D12HeapSlotAllocator>(g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 128);
        DSVHeapSlotAllocator = std::make_unique<TD3D12HeapSlotAllocator>(g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 128);
        SRVHeapSlotAllocator = std::make_unique<TD3D12HeapSlotAllocator>(g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128);

        DescriptorCache = std::make_unique<TD3D12DescriptorCache>(g_Device);
    }
}

TD3D12HeapSlotAllocator* TD3D12RHI::GetHeapSlotAllocator(D3D12_DESCRIPTOR_HEAP_TYPE Type)
{
    switch (Type)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
    {
        return SRVHeapSlotAllocator.get();
        break;
    }
        
    //case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
    //    break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
    {
        return RTVHeapSlotAllocator.get();
        break;
    }

    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
    {
        return DSVHeapSlotAllocator.get();
        break;
    }

    default:
        return nullptr;
    }
}
