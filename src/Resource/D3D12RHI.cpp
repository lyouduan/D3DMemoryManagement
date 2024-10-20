#include "D3D12RHI.h"
#include "stdafx.h"

namespace TD3D12RHI
{
    // memory allocator
    std::unique_ptr<TD3D12UploadBufferAllocator> UploadBufferAllocator = nullptr;
    std::unique_ptr<TD3D12DefaultBufferAllocator> DefaultBufferAllocator = nullptr;

    // heapSlot allocator
    std::unique_ptr<TD3D12HeapSlotAllocator> RTVHeapSlotAllocator = nullptr;
    std::unique_ptr<TD3D12HeapSlotAllocator> DSVHeapSlotAllocator = nullptr;
    std::unique_ptr<TD3D12HeapSlotAllocator> SRVHeapSlotAllocator = nullptr;

    // cache descriptor handle
    std::unique_ptr<TD3D12DescriptorCache> DescriptorCache = nullptr;

    void InitialzeAllocator(ID3D12Device* Device)
    {
        UploadBufferAllocator = std::make_unique<TD3D12UploadBufferAllocator>(Device);
        DefaultBufferAllocator = std::make_unique<TD3D12DefaultBufferAllocator>(Device);

        RTVHeapSlotAllocator = std::make_unique<TD3D12HeapSlotAllocator>(Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 128);
        DSVHeapSlotAllocator = std::make_unique<TD3D12HeapSlotAllocator>(Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 128);
        SRVHeapSlotAllocator = std::make_unique<TD3D12HeapSlotAllocator>(Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128);

        DescriptorCache = std::make_unique<TD3D12DescriptorCache>(Device);
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
