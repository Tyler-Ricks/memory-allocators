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

	if (pool_has_capacity(alloc_size, pool)) {
		return NULL;
	}

	void* result = pool->p_current;
	//pool->p_current = (void*) ((char*)pool->p_current + alloc_size);
	pool_bump(alloc_size, pool);


	return result;
}

void* pool_alloc(void* input, size_t alloc_size, Pool* pool) {
	if (pool->p_start == NULL || input == NULL) {
		return NULL;
	}

	if (pool_has_capacity(alloc_size, pool)) {
		return NULL;
	}

	void* result = pool->p_current;
	memcpy(result, input, alloc_size);
	pool_bump(alloc_size, pool);

	return result;
}

void pool_free(Pool* pool) {

}