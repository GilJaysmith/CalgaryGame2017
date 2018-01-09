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
	void* mem = Memory::s_Allocators[allocator]->Allocate(size + 4);
	int* mem_i = (int*)mem;
	*mem_i = int(allocator);
	return mem_i + 1;
}

