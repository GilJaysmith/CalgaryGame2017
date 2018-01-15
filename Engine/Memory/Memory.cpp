#include "Pch.h"

#include "Memory/Memory.h"
#include "Memory/Allocators/FreeStoreAllocator.h"

namespace Memory
{
	std::vector<Allocator*> s_Allocators(MemoryPool::COUNT);
	std::list<MemoryPool::TYPE> s_AllocatorStack = { MemoryPool::Default };

	Allocator* CreateFreeStoreAllocator()
	{
		return new FreeStoreAllocator;
	}

	Allocator* CreateDougLeaAllocator()
	{
		return nullptr;
	}

	void RegisterAllocator(MemoryPool::TYPE memory_pool_type, Allocator* allocator)
	{
		s_Allocators[memory_pool_type] = allocator;
	}
}


void* operator new(size_t size, MemoryPool::TYPE allocator)
{
	void* mem = Memory::s_Allocators[allocator]->Allocate(size + 16);
	int* mem_i = (int*)mem;
	*mem_i = int(allocator);
	return mem_i + 4;
}

void* MemAlloc(MemoryPool::TYPE allocator, size_t size)
{
	return operator new(size, allocator);
}

void MemFree(void* ptr)
{
	int* ptr_i = (int*)ptr;
	MemoryPool::TYPE allocator = (MemoryPool::TYPE)(ptr_i[-4]);
	Memory::s_Allocators[allocator]->Destroy(ptr_i - 4);
}
