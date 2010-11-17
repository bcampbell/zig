

#include "mempool.h"
#ifndef NDEBUG
#include <cstdio>
#endif


MemPool::MemPool( size_t maxchunksize, int growquanta ) :
	m_GrowQuanta( growquanta ),
	m_MaxChunkSize( maxchunksize )
{
#ifndef NDEBUG
	m_BiggestChunk = 0;
	m_MaxUsed = 0;
	m_Used = 0;
#endif

	m_BulkAllocs.reserve(16);

	Grow();
}

MemPool::~MemPool()
{
#ifndef NDEBUG
	assert( m_Used==0 );		// leaky?
//	printf( "MemPool debug report: MaxUsed=%d, m_BiggestChunk=%d "
//		"in %d bulkallocs\n",
//		m_MaxUsed, m_BiggestChunk, m_BulkAllocs.size() );
#endif

	// free all the bulk allocations
	while( !m_BulkAllocs.empty() )
	{
		delete [] m_BulkAllocs.back();
		m_BulkAllocs.pop_back();
	}
}

// add a bulk allocation of chunks
void MemPool::Grow()
{
	// add enough memory for GrowQuanta number of chunks
	m_BulkAllocs.push_back( new uint8_t[ m_MaxChunkSize * m_GrowQuanta ] );

	// add the chunks to the freelist
	int i;
	uint8_t* p = m_BulkAllocs.back();
	for( i=0; i<m_GrowQuanta; ++i )
	{
		m_Free.push_back( p );
		p += m_MaxChunkSize;
	}
}

