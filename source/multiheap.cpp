#include "include/Memory.h"
#include <new>
#include <array>

constexpr bool momOverlayExists = false; // Set this to true when using MOM

constexpr unsigned unusedRegionStart = 0x023c4000;
constexpr unsigned unusedRegionEnd   = 0x023ff000;
constexpr unsigned momOverlayStart   = 0x023ec400;

constexpr unsigned extraHeapEnd = momOverlayExists ? momOverlayStart : unusedRegionEnd;
constexpr unsigned extraHeapSize = extraHeapEnd - unusedRegionStart - sizeof(ExpandingHeap);

class MultiHeap : public ExpandingHeap
{
	static std::array<char, sizeof(ExpandingHeap) + extraHeapSize> storage;

	static ExpandingHeap& GetExtraHeap()
	{
		return *std::launder(reinterpret_cast<ExpandingHeap*>(storage.begin()));
	}

	static bool IsInExtraHeap(void* ptr)
	{
		static constexpr auto less = std::less<void*>{};

		return !less(ptr, storage.begin()) && less(ptr, storage.end());
	}

	ExpandingHeap& GetHeap(void* ptr)
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

	virtual unsigned VSizeof(void* ptr) override
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
	void* const extraHeapStart = storage.begin() + sizeof(ExpandingHeap);
	auto* extraHeapAllocator = Heap::CreateExpandingHeapAllocator(extraHeapStart, extraHeapSize, 3);

	if (!extraHeapAllocator) Crash();

	new (storage.begin()) ExpandingHeap(extraHeapStart, extraHeapSize, nullptr, extraHeapAllocator);
}

asm(R"(
_ZN9MultiHeap7storageE = 0x023c4000 @ the start of the unused region
repl_0203c948 = _ZN9MultiHeapC1EPvjP4HeapP22ExpandingHeapAllocator
)");
