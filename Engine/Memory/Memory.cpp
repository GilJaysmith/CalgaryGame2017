#include "Engine/Pch.h"

#include "Engine/Memory/Memory.h"
#include "Engine/Memory/Allocators/FreeStoreAllocator.h"

#include <set>

namespace Memory
{
	std::vector<Allocator*> s_Allocators(MemoryPool::COUNT);
	std::list<MemoryPool::TYPE> s_AllocatorStack = { MemoryPool::Default };
	std::vector<std::map<void*, std::pair<const char*, int>>> s_Allocations(MemoryPool::COUNT);

	void Initialize()
	{

	}

	void Terminate()
	{
		for (int i = 0; i < s_Allocations.size(); ++i)
		{
			std::map<void*, std::pair<const char*, int>>& allocations = s_Allocations[i];
			for (auto allocation_it : allocations)
			{
				int* ptr_i = (int*)allocation_it.first;
				int allocator = *ptr_i++;
				int size = *ptr_i++;
				const char* filename = allocation_it.second.first;
				int line = allocation_it.second.second;
				std::stringstream s;
				s << "Allocator " << allocator << ": leak at " << filename << "(" << line << ")" << " size " << size;
				Logging::Log("Memory", s.str());
			}
		}

		for (auto allocator : s_Allocators)
		{
			if (allocator)
			{
				delete allocator;
			}
		}
	}

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

void* MemAlloc(MemoryPool::TYPE allocator, size_t size, const char* filename, int line)
{
	void* ptr = Memory::s_Allocators[allocator]->Allocate(size + 4 * sizeof(int*));
	Memory::s_Allocations[allocator][ptr] = std::pair<const char*, int>(filename, line);
	int* mem_i = (int*)ptr;
	*mem_i++ = int(allocator);
	*mem_i++ = (int)size;
	mem_i++;
	mem_i++;
	return mem_i;
}

void MemFreeNotForPublicUse(void* ptr)
{
	int* ptr_i = (int*)ptr;
	ptr_i -= 4;
	MemoryPool::TYPE allocator = (MemoryPool::TYPE)(*ptr_i);
	Memory::s_Allocators[allocator]->Destroy(ptr_i);
	Memory::s_Allocations[allocator].erase(ptr_i);
}

void* operator new(size_t size, MemoryPool::TYPE allocator, const char* filename, int line)
{
	void* mem = MemAlloc(allocator, size, filename, line);
	return mem;
}

void operator delete(void*, MemoryPool::TYPE allocator, const char* filename, int line)
{

}
