#include "StdAfx.h"
#include "xtMemPoolTest.h"
#include "xtMemPool.h"
#include <vector>

xtMemPoolTest::xtMemPoolTest(void)
{
}


xtMemPoolTest::~xtMemPoolTest(void)
{
}

struct xtTestClass
{
	int a[3];
	xtTestClass *next;
};

void TestMemPool()
{
	xtMemPool<xtTestClass> tcMPool;
	xtTestClass *t0 = tcMPool.Alloc();
	t0->a[0] = 0;
	t0->a[1] = 1;
	t0->a[2] = 2;
	t0->next = NULL;
	tcMPool.Free(t0);

	const int nallocated = 1000;
	std::vector<xtTestClass *> ptrpool;
	ptrpool.reserve(nallocated);
	for ( int i=0; i<nallocated; i++ ) {
		ptrpool.push_back( tcMPool.Alloc() );
	}
	for ( int i=0; i<nallocated; ++i ) {
		tcMPool.Free( ptrpool[i] );
	} 
}
