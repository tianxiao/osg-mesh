#pragma once
#include <algorithm>

typedef unsigned char xtByte;

/**
* I copy the memory pool for cork project
* After read his design
* I also read the opensubdivision's mempool implementation
* It use the list to cache the memory
*/

template<typename T>
class xtMemPool
{
public:
	xtMemPool(int minInitBlocks=10);
	xtMemPool(xtMemPool &&src);
	~xtMemPool(void);

	void Clear();

	void operator=(xtMemPool &&);

private:
	// hide the default constructor and the operator = assign
	xtMemPool(const xtMemPool & ) {};
	xtMemPool &operator=(const xtMemPool &) { return *this; };

public:
	T *Alloc();
	void Free(T *);

private:
	union xtBlock {
		xtByte data[sizeof(T)];
		xtBlock *next;
	};
	struct xtChunk {
		xtBlock *blocks;
		int nBlocks;
		xtChunk *next;
	};
private:
	xtChunk *m_ChunkList;
	xtBlock *m_FreeList;
private:
	void AddChunk();
};

template<typename T>
xtMemPool<T>::xtMemPool(int minInitBlocks)
{
	const int MIN_BLOCKS = 2;
	int nBlocks = (minInitBlocks>MIN_BLOCKS)?minInitBlocks:MIN_BLOCKS;

	this->m_ChunkList = (xtChunk*)(new xtByte[sizeof(xtChunk)]);
	this->m_ChunkList->next = NULL;
	this->m_ChunkList->nBlocks = nBlocks;
	this->m_ChunkList->blocks = (xtBlock *)(new xtByte[sizeof(xtBlock)*nBlocks]);

	this->m_FreeList = this->m_ChunkList->blocks;
	xtBlock *lastblock = this->m_ChunkList->blocks + nBlocks - 1;
	for ( xtBlock *it=this->m_FreeList; it!=lastblock; it++ ) {
		it->next = it+1;
	}
	lastblock->next = NULL;
}

template<typename T>
xtMemPool<T>::~xtMemPool(void)
{
	xtChunk *chunk;
	while ( m_ChunkList->next!=NULL ) {
		chunk = m_ChunkList->next;
		delete [] (xtByte*)m_ChunkList->blocks;
		delete [] (xtByte*)m_ChunkList;
		m_ChunkList = chunk;
	}
	m_ChunkList = NULL;
	m_FreeList = NULL;
}

template<typename T>
xtMemPool<T>::xtMemPool(xtMemPool &&src)
{
	this->m_ChunkList = src->m_ChunkList;
	this->m_FreeList = src->m_FreeList;

	src->m_ChunkList = NULL;
	src->m_FreeList = NULL;
}


/**
* The following code explain why always add item to the front not the last one
* Cause one need use the linear search to locate the last one
	xtChunk *lastchunk = this->m_ChunkList;
	while ( lastchunk->next!=NULL ) {
		lastchunk=lastchunk->next;
	}
*/
template<typename T>
void xtMemPool<T>::AddChunk()
{
	xtChunk *lastchunk = this->m_ChunkList;
	while ( lastchunk->next!=NULL ) {
		lastchunk=lastchunk->next;
	}
	lastchunk->next		= (xtChunk*) ( new xtByte[sizeof(xtChunk)] );
	const int nblocks	= lastchunk->nBlocks*2; // logrithm increase
	lastchunk = lastchunk->next;
	lastchunk->nBlocks	= nblocks;
	lastchunk->blocks	= (xtBlock*) ( new xtByte[sizeof(xtBlock)*nblocks] );
	lastchunk->next     = NULL;

	this->m_FreeList = lastchunk->blocks;

	xtBlock *it = lastchunk->blocks;
	for ( ; it!=lastchunk->blocks+nblocks; it++ ) {
		it->next = it+1;
	}
	it->next = NULL;
}

template<typename T>
T *xtMemPool<T>::Alloc()
{
	if ( !m_FreeList ) {
		AddChunk();
	}

	T *ret = (T*)m_FreeList;
	m_FreeList = m_FreeList->next;
	return ret;
}

template<typename T>
void xtMemPool<T>::Free(T *t)
{
	xtBlock* temp = (xtBlock*)t;
	temp->next = m_FreeList;
	m_FreeList = temp;
	t=NULL;
}


template<typename T>
void xtMemPool<T>::Clear()
{
	m_FreeList = m_ChunkList->blocks;

}
