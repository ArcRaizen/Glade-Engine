#include "MemoryPool.h"
using namespace Glade;

MemoryPool::MemoryPool(size_t sizeOfEachBlock, unsigned int numOfBlocks) : numBlocks(numOfBlocks), sizeOfBlock(sizeOfEachBlock), 
	numFreeBlocks(numBlocks), numInitBlocks(0)
{
	memStart = (unsigned char*)malloc(sizeOfEachBlock * numBlocks);
	next = memStart;
}

MemoryPool::~MemoryPool()
{
	DestroyPool();
}

void MemoryPool::DestroyPool()
{
	free(memStart);
	memStart = nullptr;
}

void* MemoryPool::Allocate()
{
	// Until all blocks have been initialized once, set blocks to be used in order.
	if(numInitBlocks < numBlocks)
	{
		// Get address of next unused block
		unsigned int* p = (unsigned int*)GetAddressFromIndex(numInitBlocks);
		*p = numInitBlocks + 1;		// Set value of next unused block to index of the following block (the block to be used after it is used)
		++numInitBlocks;			// One less block available to use
	}

	void* ret = nullptr;
	if(numFreeBlocks > 0)
	{
		ret = (void*)next;		// save address of block to be used
		--numFreeBlocks;		// one less open block to be used
		if(numFreeBlocks != 0)
			next = GetAddressFromIndex(*((unsigned int*)next));	// read index of next open block (saved in block being returned)
		else													// save address of block at that index as next block to be used
			next = nullptr;	// no free blocks left
	}

	// return address of block to be used (or nullptr if all blocks are in use)
	return ret;
}

void MemoryPool::Deallocate(void* p)
{
	if(next != nullptr)
	{
		(*(unsigned int*)p) = GetIndexFromAddress(next);	// Set deallocated block to index of next free block
		next = (unsigned char*)p;					// Save deallocated block as next free block to be used
	}
	else
	{
		*((unsigned int*)p) = numBlocks;	// Set deallocated block to index at end of memory pool (no more blocks to use)
		next = (unsigned char*)p;			// Save deallocated block as next free block to be used
	}

	// One additional block free now that this one is not being used anymore
	++numFreeBlocks;
}

void MemoryPool::DeallocateAll()
{
	numFreeBlocks = numBlocks;
	numInitBlocks = 0;
	next = memStart;
}

unsigned char* MemoryPool::GetAddressFromIndex(unsigned int i) const
{
	return memStart + i * sizeOfBlock;
}

unsigned int MemoryPool::GetIndexFromAddress(const unsigned char* c) const
{
	auto t = c - memStart;
	AssertMsg((int)t > 0, "Memory Address to Deallocate is outside pool");
	AssertMsg((int)t < numBlocks, "MemoryAddress to Deallocate is outside pool");
	return ((unsigned int)(t)) / sizeOfBlock;
}
