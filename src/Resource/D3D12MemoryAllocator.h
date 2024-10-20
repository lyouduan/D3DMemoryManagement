#pragma once

#include "D3D12Resource.h"
#include <vector>
#include <set>
#include <memory>

#define DEFAULT_POOL_SIZE (512 * 1024 * 512)

#define DEFAULT_RESOURCE_ALIGNMENT 4
#define UPLOAD_RESOURCE_ALIGNMENT 256


class TD3D12BuddyAllocator
{
public:
	// 分配策略
	enum class EAllocationStrategy
	{
		PlacedResource, // 申请大块堆资源分配
		ManualSubAllocation // 每次申请一块堆资源
	};

	
	struct TAllocatorInitData
	{
		EAllocationStrategy AllocationStrategy;

		D3D12_HEAP_TYPE HeapType;

		D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE; // Only for PlacedResource


		D3D12_RESOURCE_FLAGS ResourceFlags = D3D12_RESOURCE_FLAG_NONE; // Only for ManualSubResource
	};

public:
	TD3D12BuddyAllocator(ID3D12Device* InDevice, const TAllocatorInitData& InInitData);

	~TD3D12BuddyAllocator();

	bool AllocResource(uint32_t Size, uint32_t Alignment, TD3D12ResourceLocation& ResourceLocation);

	void Deallocate(TD3D12ResourceLocation& ResourceLocation);

	void CleanUpAllocations();

	ID3D12Heap* GetBackingHeap() { return BackingHeap; }

	EAllocationStrategy GetAllocationStrategy() { return InitData.AllocationStrategy; }

private:
	void Initialize();

	// 根据阶数分配空闲块
	uint32_t AllocateBlock(uint32_t Order);

	uint32_t GetSizeToAllocate(uint32_t Size, uint32_t Alignment);

	// 是否有剩余空闲块分配
	bool CanAllocate(uint32_t SizeToAllocate);

	// 单位大小
	uint32_t SizeToUnitSize(uint32_t Size) const
	{
		return (Size + (MinBlockSize - 1)) / MinBlockSize;
	}
	// 位图
	uint32_t UnitSizeToOrder(uint32_t Size) const
	{
		unsigned long Result;
		_BitScanReverse(&Result, Size + Size - 1); // ceil(log2(size))
		return Result;
	}
	// 位图-> 单位大小
	uint32_t OrderToUnitSize(uint32_t Order) const
	{
		return ((uint32_t)1) << Order;
	}

	void DeallocateInternal(const TD3D12BuddyBlockData& Block);

	void DeallocateBlock(uint32_t Offset, uint32_t Order);

	uint32_t GetBuddyOffset(const uint32_t& Offset, const uint32_t& Size)
	{
		return Offset ^ Size;
	}
	
	uint32_t GetAllocOffsetInBytes(uint32_t Offset) const { return Offset * MinBlockSize; }

private:
	TAllocatorInitData InitData;

	const uint32_t MinBlockSize = 256;

	uint32_t MaxOrder;// 伙伴算法最大阶数

	uint32_t TotalAllocSize = 0;

	std::vector<std::set<uint32_t>> FreeBlocks; // set 平衡二叉树（黑红树）

	std::vector<TD3D12BuddyBlockData> DeferredDeletionQueue; // 延迟删除队列

	ID3D12Device* D3DDevice; 

	TD3D12Resource* BackingResource = nullptr; // for createCommittedResource

	ID3D12Heap* BackingHeap = nullptr; // for createHeap
};

class TD3D12MultiBuddyAllocator
{
public:
	TD3D12MultiBuddyAllocator(ID3D12Device* InDevice, const TD3D12BuddyAllocator::TAllocatorInitData& InInitData);

	~TD3D12MultiBuddyAllocator();

	bool AllocResource(uint32_t Size, uint32_t Alignment, TD3D12ResourceLocation& ResourceLocation);

	void CleanUpAllocations();

private:
	std::vector<std::shared_ptr<TD3D12BuddyAllocator>> Allocators;

	ID3D12Device* Device;

	TD3D12BuddyAllocator::TAllocatorInitData InitData;
};

class TD3D12UploadBufferAllocator
{
public:
	TD3D12UploadBufferAllocator(ID3D12Device* InDevice);

	void* AllocUploadResource(uint32_t Size, uint32_t Alignment, TD3D12ResourceLocation& ResourceLocation);

	void CleanUpAllocations();

private:
	std::unique_ptr<TD3D12MultiBuddyAllocator> Allocator = nullptr;

	ID3D12Device* D3DDevice = nullptr;
};

class TD3D12DefaultBufferAllocator
{
public:
	TD3D12DefaultBufferAllocator(ID3D12Device* InDevice);

	void AllocDefaultResource(const D3D12_RESOURCE_DESC& ResourceDesc, uint32_t Alignment, TD3D12ResourceLocation& ResourceLocation);

	void CleanUpAllocations();

private:
	std::unique_ptr<TD3D12MultiBuddyAllocator> Allocator = nullptr;

	std::unique_ptr<TD3D12MultiBuddyAllocator> UavAllocator = nullptr;

	ID3D12Device* D3DDevice = nullptr;
};

class TD3D3TextureResourceAllocator
{
public:
	TD3D3TextureResourceAllocator(ID3D12Device* InDevice);

	void AllocTextureResource(const D3D12_RESOURCE_STATES& ResourceState, const D3D12_RESOURCE_DESC& ResourceDesc, TD3D12ResourceLocation& ResourceLocation);

	void CleanUpAllocations();

private:
	std::unique_ptr<TD3D12MultiBuddyAllocator> Allocator = nullptr;

	ID3D12Device* D3DDevice = nullptr;
};