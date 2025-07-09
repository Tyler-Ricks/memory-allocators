#include "Pool.h"

// allocates memory for a pool of size _size_
// fills in the properties of the pool pointer that is passed in
// 
// Pool* new_pool{};
// if(pool_create(sizeof(float) * 100, &new_pool) != POOL_SUCCESS){
//     printf("failed to allocate memory for a pool!");
// }
POOL_RESULT pool_create(size_t size, Pool* p_pool) {
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