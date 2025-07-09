#include "Pool.h"

// allocates memory for a pool of size _size_
// fills in the properties of the pool pointer that is passed in
// 
// Pool* new_pool{};
// if(pool_create(sizeof(float) * 100, &new_pool) != POOL_SUCCESS){
//     printf("failed to allocate memory for a pool!");
// }

/*
POOL_RESULT pool_create(size_t size, Pool* p_pool) {
	if (p_pool->p_start != 0) {
		return POOL_FAIL;
	}

	void* memory = malloc(size);

	if (memory == NULL) {
		return POOL_FAIL;
	}
	
	p_pool->p_start = memory;
	p_pool->p_current = memory;
	p_pool->size = size;
	p_pool->p_next = NULL;

	return POOL_SUCCESS;
}
*/

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