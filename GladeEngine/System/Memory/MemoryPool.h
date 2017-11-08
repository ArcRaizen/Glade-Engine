#pragma once
#ifndef GLADE_MEMORY_POOL_H
#define GLADE_MEMORY_POOL_H
#include "../../Utils/Assert.h"
#include <stdlib.h>

// Allocate default pool of some value (1kb?) to start program as default pool.
	// Override new/delete to default pool?
	// Override new/delete to named pool?

namespace Glade {
/*
	Simple Memory Pool
	Allocates in blocks of a specified size (sizeOfBlock) and holds specified number of blocks (numBlocks)
	Can only allocate blocks of specified size and not other sizes
	Requires manual management of pool by calling routine
*/
class MemoryPool
{
public:
	MemoryPool(size_t sizeOfEachBlock, unsigned int numOfBlocks);	
	~MemoryPool();
	void DestroyPool();

	// Return pointer to valid memory location in pool to be used to store data
	void* Allocate();

	// Allow block and memory address 'p' inside pool to be used later to store new data
	void Deallocate(void* p);

	// Reset pool to as if it was never used to make use of it fresh
	void DeallocateAll();

	// Return memory address of of block number 'i' (starting from 0) in the memory pool
	unsigned char* GetAddressFromIndex(unsigned int i) const;

	// Return the block number (starting from 0) of the given memory adress in the pool
	unsigned int GetIndexFromAddress(const unsigned char* c) const;

private:
	unsigned int	numBlocks;		// Number of Blocks
	unsigned int	sizeOfBlock;	// Size of each Block
	unsigned int	numFreeBlocks;	// Number of remaining Blocks
	unsigned int	numInitBlocks;	// Number of initialized Blocks
	unsigned char*	memStart;		// Beginning of memory pool
	unsigned char*	next;			// Next free Block
};
}	// namespace Glade
#endif	// GLADE_MEMORY_POOL
