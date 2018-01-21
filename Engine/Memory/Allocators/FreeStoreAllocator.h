#pragma once

#include "Engine/Memory/Memory.h"

namespace Memory
{
	class FreeStoreAllocator : public Allocator
	{
	public:
		void* Allocate(std::size_t size) override {
			return malloc(size);
		}
		void Destroy(void* ptr) override {
			free(ptr);
		}
	};
}
