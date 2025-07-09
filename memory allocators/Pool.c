#include "Pool.h"

// returns true (1) if _pool_ has room for an allocation of size _alloc_size_
// returns false (0) otherwise.
inline POOL_BOOL pool_has_capacity(size_t alloc_size, Pool* pool) {
	size_t old_size = (char*) pool->p_current - (char*) pool->p_start;
	return(pool->size >= old_size + alloc_size);
}

inline void pool_bump(size_t alloc_size, Pool* pool) {
	pool->p_current = (char*) pool->p_current + alloc_size;
}

// function for determining a new size for a pool.
// for now, just multiply old size by POOL_GROWTH_FACTOR by default
// returns the size of the new pool if a pool can be allocated without
// exceeding POOL_SIZE_CAP
// returns 0 if not
inline size_t pool_new_size(size_t alloc_size, Pool* pool) {
	if (alloc_size > POOL_SIZE_CAP) { return 0; }

	size_t new_size = pool->size * POOL_GROWTH_FACTOR;

	if (new_size < alloc_size) {
		new_size = POOL_GROWTH_FACTOR * (alloc_size * pool->size);
	}
	if (new_size > POOL_SIZE_CAP) {
		new_size = POOL_SIZE_CAP;
	}

	return new_size;
}

void pool_print(Pool* pool) {
	if (pool == NULL) {
		printf("pool is null\n\n");
		return;
	}
	if (pool->p_current == NULL) {
		printf("start: NULL\ncurrent: NULL\nsize: 0\nnext: NULL\n\n");
		return;
	}

	printf("start: %p\ncurrent: %p\nsize: %zu\n", pool->p_start, pool->p_current, pool->size);
	if (pool->p_next == NULL) { 
		printf("next: NULL\n\n");
		return;
	}
	printf("next: %p\n\n", pool->p_next);
}

// pool creators:

// allocates a chunk of memory of size _size_, then returns a pool with a pointer to that memory
// returns POOL_ERROR on failure
//
// Pool new_pool = pool_create(sizeof(float) * 100);
Pool pool_create(size_t size) {
	void* memory = malloc(size);

	if (memory == NULL) {
		return POOL_ERROR;
	}

	return (Pool) {
		memory,
		memory,
		size, 
		NULL
	};
}

// allocates space for a pool, but also for the member variables of a Pool struct
Pool* pool_heap_create(size_t size) {
	return NULL;
}


// things that allocate new pools if an old one runs out of capacity

// creates a new pool of a size big enough for the input, then attaches it to
// the end of the linked list of pools
// returns a pointer to the new pool upon success
// returns NULL upon failure
Pool* pool_realloc(size_t alloc_size, Pool* pool) {
	while (pool->p_next != NULL) {
		pool = pool->p_next;
	}

	size_t new_size = pool_new_size(alloc_size, pool);
	if(new_size == 0){ return NULL; }

	Pool* new_pool = pool_heap_create(new_size);
}


// things that allocate to pools

// allocates _alloc_size_ bytes from _pool_
// returns a pointer to the start of the allocated memory
// bumps _pool_'s current ptr to the next free location
//
// float* var = pool_alloc(sizeof(float), pool)
// *var = 25.0;
void* pool_raw_alloc(size_t alloc_size, Pool* pool) {
	if (pool->p_start == NULL) {
		return NULL;
	}

	if (!pool_has_capacity(alloc_size, pool)) {
		return NULL;
	}

	void* result = pool->p_current;
	//pool->p_current = (void*) ((char*)pool->p_current + alloc_size);
	pool_bump(alloc_size, pool);


	return result;
}

void* pool_alloc(void* data, size_t alloc_size, Pool* pool) {
	if (pool->p_start == NULL || data == NULL) {
		return NULL;
	}

	if (!pool_has_capacity(alloc_size, pool)) {
		return NULL;
	}

	void* result = pool->p_current;
	memcpy(result, data, alloc_size);
	pool_bump(alloc_size, pool);

	return result;
}


// stuff that frees pools:

// frees _pool_ and all pools it has attached to it with p_next
void pool_free(Pool* pool) {
	if (pool->p_start == NULL) {
		return;
	}

	free(pool->p_start);
}