#include "ManagedMemoryPool.h"
using namespace Glade;

ManagedMemoryPool::ManagedMemoryPool(const std::string& name, size_t sizeOfEachBlock, unsigned int numOfBlocks) : numBlocks(numOfBlocks), sizeOfBlock(sizeOfEachBlock), 
	numFreeBlocks(numBlocks), numInitBlocks(0), Resource(name)
{
	memStart = (unsigned char*)malloc(sizeOfEachBlock * numBlocks);
	next = memStart;
}

SmartPointer<ManagedMemoryPool> ManagedMemoryPool::CreatePool(const std::string& name, size_t sizeOfEachBlock, unsigned int numOfBlocks)
{
	auto pool = SmartPointer<ManagedMemoryPool>(new ManagedMemoryPool(name.length()>0 ? name : GenerateName("Unnamed MemoryPool %i"), sizeOfEachBlock, numOfBlocks));
	if(pool)
		RegisterSmartResource(pool);
	return pool;
}

ManagedMemoryPool::~ManagedMemoryPool()
{
	DestroyPool();
}

void ManagedMemoryPool::DestroyPool()
{
	free(memStart);
	memStart = nullptr;
}

void* ManagedMemoryPool::Allocate(const std::string& name, bool createNewIfFull/*=false*/)
{
	auto pool = FindResourceByName(name);

	// Pool of given name does not exist
	if(pool == nullptr)
		return nullptr;

	void* p = pool->Allocate();
	if(p == nullptr && createNewIfFull)	// Pool found, but no space to further allocate memory
	{
		// Pick name for new pool based on original name
		size_t lastIndex = name.find_last_not_of("0123456789");
		std::string n = lastIndex == std::string::npos ? "1" : name.substr(lastIndex + 1);
		int counter = n == "" ? 2 : std::stoi(n) + 1;

		// If pools "P" and "P2" exist and the passed-in name is "P" and pool "P" is full, there's a problem
		// The new name picked will be "P2" and that already exists, so we'd create 2 pools with the same name
		// Loop until we're sure no pool exists with the given name and counter
		std::string newName;
		do
		{
			newName = name.substr(0, lastIndex+1) + std::to_string(counter++);
		}while(FindResourceByName(newName) != nullptr);
		
		// Create new pool
		pool = CreatePool(newName, pool->sizeOfBlock, pool->numBlocks);
		p = pool->Allocate();
	}

	return p;
}

void* ManagedMemoryPool::Allocate()
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

void ManagedMemoryPool::Deallocate(void* p)
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

void ManagedMemoryPool::DeallocateAll()
{
	numFreeBlocks = numBlocks;
	numInitBlocks = 0;
	next = memStart;
}

unsigned char* ManagedMemoryPool::GetAddressFromIndex(unsigned int i) const
{
	return memStart + i * sizeOfBlock;
}

unsigned int ManagedMemoryPool::GetIndexFromAddress(const unsigned char* c) const
{
	auto t = c - memStart;
	AssertMsg((int)t > 0, "Memory Address to Deallocate is outside pool");
	AssertMsg((int)t < numBlocks, "MemoryAddress to Deallocate is outside pool");
	return ((unsigned int)(t)) / sizeOfBlock;
}
