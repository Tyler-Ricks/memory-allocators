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

// TODO
// #  | (done) | task
// 1  |        | pool_create() for creating a pool
// 2  |		   | pool_alloc() for allocating memory from a pool

#ifndef POOL_H
#define POOL_H

#include <stdio.h>
#include <string.h>

#define POOL_SIZE_CAP 16000
#define POOL_GROWTH_FACTOR 1.5f

typedef struct {
	const void* p_start;	// pointer to the start of the pool
	void* p_current;		// pointer to the next free address
	const size_t size;		// size of the pool in bytes
	Pool* p_next;			// pointer to the next pool. NULL if there is none
}Pool;


#endif POOL_H