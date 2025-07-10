#include "Pool.h"

// returns true (1) if _pool_ has room for an allocation of size _alloc_size_
// returns false (0) otherwise.
inline POOL_BOOL pool_has_capacity(const size_t alloc_size, Pool* p_pool) {
	size_t old_size = (char*) p_pool->p_current - (char*) p_pool->p_start;
	return(p_pool->size >= old_size + alloc_size);
}

inline void pool_bump(const size_t alloc_size, Pool* p_pool) {
	p_pool->p_current = (char*) p_pool->p_current + alloc_size;
}

// function for determining a new size for a pool.
// for now, just multiply old size by POOL_GROWTH_FACTOR by default
// returns the size of the new pool if a pool can be allocated without
// exceeding POOL_SIZE_CAP
// returns 0 if not
inline size_t pool_new_size(const size_t alloc_size, Pool* pool) {
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

void pool_print(const Pool* pool) {
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
Pool pool_create(const size_t size) {
	if (size <= 0) {
		return POOL_ERROR;
	}

	void* p_memory = malloc(size);

	if (p_memory == NULL) {
		return POOL_ERROR;
	}

	return (Pool) {
		p_memory,	// p_start
		p_memory,	// p_current
		size,		// size
		NULL		// p_next
	};
}

// allocates space for a pool, but also for the member variables of a Pool struct
// returns a pointer to the new pool if successful, but NULL if not
Pool* pool_heap_create(const size_t size) {
	if (size <= 0) {
		return NULL;
	}
	
	void* p_memory = malloc(sizeof(Pool) + size);
	if(p_memory == NULL){ return NULL; }

	// offset the pool memory to leave room for the struct
	void* p_pool_memory_start = (char*) p_memory + sizeof(Pool); 

	Pool new_pool = {
		p_pool_memory_start,	// p_start
		p_pool_memory_start,	// p_current
		size,					// size
		NULL					// p_next
	};	

	memcpy(p_memory, &new_pool, sizeof(Pool));	// the size member is const, so we can't just assign the struct normally

	return p_memory;
}


// things that allocate new pools if an old one runs out of capacity

// creates a new pool of a size big enough for the input, then attaches it to
// the p_next member of _pool_
// returns a pointer to the new pool upon success
// returns NULL upon failure
// used by pool_has_capacity
Pool* pool_realloc(const size_t alloc_size, Pool* p_pool) {
	assert(p_pool->p_next == NULL);

	size_t new_size = pool_new_size(alloc_size, p_pool);
	if(new_size == 0){ return NULL; }

	p_pool->p_next = pool_heap_create(new_size); // pool_heap_create returns NULL upon failure
	return p_pool->p_next;
}

// Iterates through the pool linked list to find a pool with enough space for _alloc_size_
// if one is found, return a pointer to it
// if the end of the pool chain is found, allocate a new one with enough space with pool_realloc
// returns a pointer to the pool with capacity
// returns NULL if a pool isn't found, and one with capacity cannot be created
// used by pool allocators when the main pool does not have capacity
Pool* pool_find_capacity(const size_t alloc_size, Pool* p_pool) {
	if(p_pool == NULL || p_pool->p_start == NULL) { return NULL; }

	while (p_pool->p_next != NULL) {
		if(pool_has_capacity(alloc_size, p_pool->p_next)){
			return p_pool->p_next;
		}
		p_pool = p_pool->p_next;
	}

	return(pool_realloc(alloc_size, p_pool));
}


// things that allocate to pools

// allocates _alloc_size_ bytes from _pool_
// returns a pointer to the start of the allocated memory
// bumps _pool_'s current ptr to the next free location
//
// float* var = pool_alloc(sizeof(float), pool)
// *var = 25.0;
void* pool_raw_alloc(const size_t alloc_size, Pool* p_pool) {
	if (p_pool->p_start == NULL) {
		return NULL;
	}

	if (!pool_has_capacity(alloc_size, p_pool)) {
		// return NULL;
		p_pool = pool_find_capacity(alloc_size, p_pool);
		if(p_pool == NULL){ return NULL; }
	}

	void* result = p_pool->p_current;
	//pool->p_current = (void*) ((char*)pool->p_current + alloc_size);
	pool_bump(alloc_size, p_pool);


	return result;
}

void* pool_alloc(const void* data, const size_t alloc_size, Pool* p_pool) {
	if (p_pool->p_start == NULL || data == NULL) {
		return NULL;
	}

	if (!pool_has_capacity(alloc_size, p_pool)) {
		//return NULL;
		p_pool = pool_find_capacity(alloc_size, p_pool);
		if(p_pool == NULL){ return NULL; }
	}

	void* result = p_pool->p_current;
	memcpy(result, data, alloc_size);
	pool_bump(alloc_size, p_pool);

	return result;
}


// stuff that frees pools:

// frees pools stored on the heap
// used by pool_free
void pool_heap_free(Pool* p_pool) {
	while (p_pool != NULL) {
		Pool* p_old_pool = p_pool;
		p_pool = p_pool->p_next;
		free(p_old_pool);
	}
}

// frees _pool_ and all pools it has attached to it with p_next
void pool_free(Pool* p_pool) {
	if (p_pool->p_start == NULL) {
		return;
	}

	if (p_pool->p_next != NULL) {
		pool_heap_free(p_pool->p_next);
	}

	free(p_pool->p_start);

	Pool cleared_pool = {
		NULL,
		NULL,
		0,
		NULL
	};

	memcpy(p_pool, &cleared_pool, sizeof(Pool));
}