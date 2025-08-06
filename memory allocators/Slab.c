#include "Slab.h"


Slab* slab_create(void* memory) {
	return &((Slab) {memory, NULL});
}

Slab* slab_list_create(void* memory, size_t slab_size, uint32_t slab_count) {

	Slab* current = slab_create(memory);
	Slab* result = current;
	for (size_t i = 0; i < slab_count; i++) {
		Slab* new_slab = slab_create(((char*)current->mem) + slab_size);
		current->next = new_slab;
		current = new_slab;
	}

	return result;
}

// creates a frame of slabs
Frame* frame_create(const size_t slab_size, const uint32_t slab_count) {
	if (slab_size == 0 || slab_count == 0) {
		return NULL;
	}

	void* chunk = malloc(slab_size * slab_count);
	if(chunk == NULL) { return NULL; }

	Slab* slab_list = slab_list_create(chunk, slab_size, slab_count);

	return &(Frame) {chunk, slab_size, slab_count, slab_list};
}
