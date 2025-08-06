#include "Slab.h"

// honestly this just exists in case I need to change in later
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
Frame frame_create(const size_t slab_size, const uint32_t slab_count) {
	if (slab_size == 0 || slab_count == 0) {
		return FRAME_ERROR;
	}

	void* chunk = malloc(slab_size * slab_count);
	if(chunk == NULL) { return FRAME_ERROR; }

	Slab* slab_list = slab_list_create(chunk, slab_size, slab_count);

	return (Frame) {chunk, slab_size, slab_count, slab_list};
}



void* slab_alloc_raw(Frame* frame) {
	if(frame == 0 || frame->available == NULL) {
		printf("ran out of space!\n");
		return NULL; 
	}

	Slab* slab = frame->available;
	frame->available = slab->next;
	slab->next = NULL;
	return slab->mem;
}


void* slab_alloc(void* data, Frame* frame) {
	if(frame == 0 || frame->available == NULL || data == NULL) {
		printf("ran out of space!\n");
		return NULL;
	}

	Slab* slab = frame->available;
	frame->available = slab->next;
	slab->next = NULL;

	memcpy(slab->mem, data, frame->slab_size);
	return slab->mem;
}


// 0s out the memory from the slab to be freed, then adds it to the start 
// of the LL of available slabs
void slab_free(void* location, Frame* frame) {
	if(frame == 0 || location == NULL) { return NULL; }
	
	// zeroing out the old memory COULD be optional.
	// less safe of course, but saves time (though memset is pretty fast)
	memset(location, 0, frame->slab_size);
	Slab* new_slab = slab_create(location);
	new_slab->next = frame->available;
	frame->available = new_slab;
}

void frame_free(Frame* frame) {
	if(frame == NULL){ return; }

	free(frame->start);

	frame->start = NULL;
	frame->available;
}
