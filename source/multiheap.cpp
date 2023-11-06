#include "include/Memory.h"
#include <new>
#include <array>

// Set this to true if your hack uses the More Objects Mod, or if you aren't sure if it does.
// Only set it to false if you're certain that it doesn't.
#define MOM_OVERLAY_MAY_EXIST true

struct BaseWithoutMOM
{
	static constexpr char* extraHeapStorage = UNUSED_RAM_REGION;
	static constexpr unsigned extraHeapSize = sizeof(UNUSED_RAM_REGION) - sizeof(ExpandingHeap);
};

struct BaseWithMOM
{
	char* extraHeapStorage = BaseWithoutMOM::extraHeapStorage;
	unsigned extraHeapSize = BaseWithoutMOM::extraHeapSize;

	BaseWithMOM()
	{
		InitFileSystem();

		OverlayInfo ovInfo;
		const bool momExists = LoadOverlayInfo(ovInfo, false, 155);

		if (momExists)
		{
			const unsigned momOverlaySize = ovInfo.loadSize + ovInfo.bssSize;

			extraHeapStorage += momOverlaySize;
			extraHeapSize -= momOverlaySize;
		}
	}
};

class MultiHeap : public ExpandingHeap, std::conditional_t<MOM_OVERLAY_MAY_EXIST, BaseWithMOM, BaseWithoutMOM>
{
	ExpandingHeap& GetExtraHeap()
	{
		return *std::launder(reinterpret_cast<ExpandingHeap*>(extraHeapStorage));
	}

	bool IsInExtraHeap(const void* ptr)
	{
		static constexpr auto less = std::less<const void*>{};

		return !less(ptr, extraHeapStorage) && less(ptr, extraHeapStorage + extraHeapSize);
	}

	ExpandingHeap& GetHeap(const void* ptr)
	{
		if (IsInExtraHeap(ptr))
			return GetExtraHeap();
		else
			return *this;
	}

public:
	MultiHeap(void* start, unsigned size, Heap* root, ExpandingHeapAllocator* allocator);

	virtual void VDestroy() override
	{
		ExpandingHeap::VDestroy();
		GetExtraHeap().Destroy();
	}

	virtual void* VAllocate(unsigned size, int align) override
	{
		void* res = GetExtraHeap().ExpandingHeap::VAllocate(size, align);

		if (!res) res = ExpandingHeap::VAllocate(size, align);

		return res;
	}

	virtual bool VDeallocate(void* ptr) override
	{
		return GetHeap(ptr).ExpandingHeap::VDeallocate(ptr);
	}

	virtual void VDeallocateAll() override
	{
		ExpandingHeap::VDeallocateAll();
		GetExtraHeap().ExpandingHeap::VDeallocateAll();
	}

	virtual unsigned VReallocate(void* ptr, unsigned newSize) override
	{
		return GetHeap(ptr).ExpandingHeap::VReallocate(ptr, newSize);
	}

	virtual unsigned VSizeof(const void* ptr) override
	{
		return GetHeap(ptr).ExpandingHeap::VSizeof(ptr);
	}

	virtual unsigned VMaxAllocationUnitSize() override
	{
		const unsigned size1 = ExpandingHeap::VMaxAllocationUnitSize();
		const unsigned size2 = GetExtraHeap().ExpandingHeap::VMaxAllocationUnitSize();

		return std::max(size1, size2);
	}

	virtual unsigned VMaxAllocatableSize() override
	{
		const unsigned size1 = ExpandingHeap::VMaxAllocatableSize();
		const unsigned size2 = GetExtraHeap().ExpandingHeap::VMaxAllocatableSize();

		return std::max(size1, size2);
	}

	virtual unsigned VMemoryLeft() override
	{
		return ExpandingHeap::VMemoryLeft() + GetExtraHeap().ExpandingHeap::VMemoryLeft();
	}
};

MultiHeap::MultiHeap(void* start, unsigned size, Heap* root, ExpandingHeapAllocator* allocator):
	ExpandingHeap(start, size, root, allocator)
{
	void* const extraHeapStart = extraHeapStorage + sizeof(ExpandingHeap);
	auto* extraHeapAllocator = Heap::CreateExpandingHeapAllocator(extraHeapStart, extraHeapSize, 3);

	if (!extraHeapAllocator) Crash();

	new (extraHeapStorage) ExpandingHeap(extraHeapStart, extraHeapSize, nullptr, extraHeapAllocator);
}

#if MOM_OVERLAY_MAY_EXIST

asm("nsub_0201a0d8 = 0x0201a0dc");

#endif

asm("repl_0203c948 = _ZN9MultiHeapC1EPvjP4HeapP22ExpandingHeapAllocator");
