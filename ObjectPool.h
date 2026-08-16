#pragma once
#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

template <typename T>
class ObjectPool {
public:
	explicit ObjectPool(size_t initialSize = 100)
		: mGrowSize(std::max<size_t>(initialSize, 1)) {
		Expand(mGrowSize);
	}
	ObjectPool(const ObjectPool&) = delete;
	ObjectPool& operator=(const ObjectPool&) = delete;

	~ObjectPool() {
		for (const auto& block : mAllBlocks) {
			std::allocator_traits<std::allocator<T>>::deallocate(mAllocator, block.first, block.second);
		}
	}

	T* Allocate() {
		if (mFreeList.empty()) {
			Expand(mGrowSize);
		}
		T* ptr = mFreeList.back();
		mFreeList.pop_back();
		return ptr;
	}

	void Free(T* ptr) {
		if (ptr) {
			mFreeList.push_back(ptr);
		}
	}

	bool Owns(const T* ptr) const {
		if (!ptr) {
			return false;
		}
		const auto address = reinterpret_cast<std::uintptr_t>(ptr);
		for (const auto& block : mAllBlocks) {
			const auto begin = reinterpret_cast<std::uintptr_t>(block.first);
			const auto end = begin + sizeof(T) * block.second;
			if (address >= begin && address < end) {
				return true;
			}
		}
		return false;
	}

private:
	// 増設
	void Expand(size_t size) {
		size = std::max<size_t>(size, 1);
		T* newBlock = std::allocator_traits<std::allocator<T>>::allocate(mAllocator, size);
		for (size_t i = 0; i < size; ++i) {
			mFreeList.push_back(&newBlock[i]);
		}
		mAllBlocks.emplace_back(newBlock, size);
	}

	std::allocator<T> mAllocator;
	std::vector<T*> mFreeList;
	std::vector<std::pair<T*, size_t>> mAllBlocks;
	size_t mGrowSize;
};
