

#ifndef MEMPOOL_H
#define MEMPOOL_H

#include "zigtypes.h"
#include <assert.h>
#include <list>
#include <vector>
#include <cstddef>  // for size_t

// Use MemPool to provide low-overhead allocation and freeing of
// small objects. All the objects allocated must be smaller than
// maxchunksize bytes.

class MemPool
{
public:
	MemPool( size_t maxchunksize, int growquanta );
	~MemPool();

	// numbytes must be <= maxchunksize
	void* Allocate( size_t numbytes );

	void Free( void* chunk );

private:
	// not allowed
	MemPool();
	MemPool( const MemPool& );

	void Grow();
	
	std::vector< uint8_t* > m_BulkAllocs;
	int						m_GrowQuanta;

	size_t					m_MaxChunkSize;


	// list of free chunks
	// (should use slist if available)
	std::list<void*>		m_Free;

#ifndef NDEBUG
	// stats for debugging
	size_t					m_BiggestChunk;
	int						m_MaxUsed;
	int						m_Used;
#endif
};



inline void* MemPool::Allocate( size_t numbytes )
{
	assert( numbytes < m_MaxChunkSize );	// max chunksize needs increasing!

	// if we've run out of chunks, get some more
	if( m_Free.empty() )
		Grow();
	assert( !m_Free.empty() );

	// return the next available one.
	uint8_t* chunk = (uint8_t*)m_Free.back();
	m_Free.pop_back();

#ifndef NDEBUG
	++m_Used;
	// collect some debug stats
	if( numbytes > m_BiggestChunk )
		m_BiggestChunk = numbytes;
	if( m_Used > m_MaxUsed )
		m_MaxUsed = m_Used;
#endif

	return chunk;
}

inline void MemPool::Free( void* chunk )
{
	m_Free.push_back( chunk );
#ifndef NDEBUG
	--m_Used;
	assert( m_Used >= 0 );
#endif
}

#endif // MEMPOOL_H


