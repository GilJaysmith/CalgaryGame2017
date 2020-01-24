#include "Engine/Pch.h"

#include "Engine/Memory/Allocators/FreeStoreAllocator.h"


namespace Memory
{
	void* FreeStoreAllocator::Allocate(std::size_t size)
	{
		void* ptr = malloc(size);
		return ptr;
	}

	void FreeStoreAllocator::Destroy(void* ptr)
	{
		free(ptr);
	}
}

