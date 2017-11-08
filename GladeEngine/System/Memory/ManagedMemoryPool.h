#pragma once
#ifndef GLADE_MANAGED_MEMORY_POOL_H
#define GLADE_MANAGED_MEMORY_POOL_H
#include "../Resource.h"
#include <stdlib.h>

namespace Glade {
/*
	Memory Pools that have a bit of automatic management behind the scenes.
	Pools are given names and automatically stored in static unordered_maps via Resource with their name
	Pools are automatically managed by SmartPointers so they are never destroyed unless requested
	Any code can gain access to a pool as long as the name of said pool is known.
	Extra static Allocate function provided that can create new pools if requested pool is full

	Not meant to be the most efficient memory management system, but a bit easier and intuitive for people
	not totally familiar with memory management or worried about the most optimal code.
*/
class ManagedMemoryPool : public Resource<ManagedMemoryPool>
{
	ManagedMemoryPool(const std::string& name, size_t sizeOfEachBlock, unsigned int numOfBlocks);

public:
	static SmartPointer<ManagedMemoryPool> CreatePool(const std::string& name, size_t sizeOfEachBlock, unsigned int numOfBlocks);
	
	~ManagedMemoryPool();
	void DestroyPool();

	// Allocate requested memory from specific pool and create new pool if requested is full
	// Returns nullptr if no pool of given name exists, valid memory address to use otherwise
	// If pool with given name is full and a new pool is created, "name" is set to the name of the new pool
	/* Use when:
		1) You don't want to manage pools manually (let it happen behind the scenes)
		2) You need something allocated guaranteed with little hassle for the calling routine
		3) You can't be sure of the proper size of your pools, so you created them small and are willing to make extra as needed
		4) You don't know what's going on here but want to use pools
	 */
	static void* Allocate(const std::string& name, bool createNewIfFull=false);

	// Return pointer to valid memory location in pool to be used to store data
	// Use if you know what you're doing
	void* Allocate();

	// Allow block and memory adress 'p' inside pool to be used later to store new data
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
#endif	// GLADE_MANAGED_MEMORY_POOL_H
