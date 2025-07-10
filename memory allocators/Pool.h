// This is a bump allocator/ pool allocator. 
// It is useful for organizing memory that is dynamically allocated into groups, so 
// you don't need to track a bunch of individual lifetimes. Also, it avoids slow sys 
// calls from malloc if you need to make many allocations quickly.
// 
// It works by first allocating a chunk of memory. Then, the user allocates memory 
// from this chunk of memory. As space in the pool is taken up, the p_current pointer
// "bumps" up to the next free spot in the pool, until there is no space left. If more
// space is needed, a new pool is allocated, and a pointer to the new pool is tracked 
// in the previous pool. When data in the entire pool is no longer needed, the entire 
// pool is freed from p_start.
// 
// Be careful with making allocations from a pool that you are planning on freeing.
// Just like with normal dynamic alloations, allocated memory is referenced with a
// pointer. Each reference to a spot in the pool is left hanging if the pool is freed

// Conventions and stuff
// |	I'll try to list some conventions I am following here.
// |
// |	any time a variable is a pointer, its name must have the "p_" prefix
// |
// |	I try to keep things that explicitly allocate/free relegated to their own functions
// |	For example, pool_realloc could allocate the memory it needs on its own, but I have
// |	it call pool_heap_create instead
// | 
// |	I try to keep things const by default. 
// |

// TODO

#ifndef POOL_H
#define POOL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define POOL_SIZE_CAP 16000
#define POOL_GROWTH_FACTOR 1.5f
#define POOL_ERROR (Pool){NULL, NULL, 0, NULL}

// these are for the case where I want a sentinel return value for functions like pool_create or pool_alloc
typedef int POOL_RESULT;
#define POOL_SUCCESS 1
#define POOL_FAIL 0
typedef int POOL_BOOL;
#define POOL_TRUE 1
#define POOL_FALSE 0

typedef struct {
	const void* p_start;	// pointer to the start of the pool
	void* p_current;		// pointer to the next free address
	const size_t size;		// size of the pool in bytes
	struct Pool* p_next;	// pointer to the next pool. NULL if there is none
}Pool;

// utilities

inline POOL_BOOL pool_has_capacity(const size_t alloc_size, Pool* p_pool);
inline void pool_bump(const size_t alloc_size, Pool* p_pool);
inline size_t pool_new_size(const size_t alloc_size, Pool* p_pool);
void pool_print(const Pool* p_pool);

// stuff that creates pools
Pool pool_create(const size_t size);
Pool* pool_heap_create(const size_t size);

// stuff that creates new pools if a pool runs out of capacity
Pool* pool_realloc(const size_t alloc_size, Pool* p_pool);
Pool* pool_find_capacity(const size_t alloc_size, Pool* p_pool);

// stuff that allocates to a pool
void* pool_raw_alloc(const size_t alloc_size, Pool* p_pool);
void* pool_alloc(const void* data, const size_t alloc_size, Pool* p_pool);

// stuff that frees pools
void pool_heap_free(Pool* p_pool);
void pool_free(Pool* p_pool);

#endif POOL_H