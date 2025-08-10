#include "Slab.h"

SLAB_RESULT frame_create(size_t slab_size, const uint32_t slab_count, Frame* frame) {

	// so at the moment, this will not work for storing types that are smaller than 
	// a pointer (such as a float), since a pointer to the next available slab is stored IN an 
	// available slab. I could either just have slabs have a minimum size equal to pointer size,
	// or maybe store an int offset in each available slab
	// for now just have a default slab size be equal to the size of a pointer
	if (slab_size == 0|| slab_count == 0 || frame == NULL) {
		return SLAB_INVALID_INPUT;
	}

	if (slab_size < sizeof(void*)) {
		slab_size = sizeof(void*);
	}

	void* chunk = malloc(slab_size * slab_count);
	if(chunk == NULL){ return SLAB_FAILURE; }

	// set data in each slab to contain a pointer to the next available slab location
	void* head = chunk;
	for (size_t i = 0; i < slab_count - 1; ++i) {
		void* next = (char*)head + slab_size;
		*(void**)head = next;
		head = next;
	}
	*(void**)head = NULL;

	frame->start = chunk;
	frame->available = chunk;
	frame->slab_size = slab_size;
	frame->slab_count = slab_count;

	return SLAB_SUCCESS;
}


void* slab_alloc_raw(Frame* frame) {
	if(frame == NULL || frame->available == NULL) {
		printf("ran out of space!\n");
		return NULL; 
	}

	void* slab = frame->available;
	frame->available = *(void**)frame->available;
	
	return slab;
}


void* slab_alloc(void* data, Frame* frame) {
	if(frame == NULL || frame->available == NULL || data == NULL) {
		printf("ran out of space!\n");
		return NULL;
	}

	void* slab = frame->available;
	frame->available = *(void**)frame->available;

	memcpy(slab, data, frame->slab_size);
	return slab;
}


uint32_t count_available_slabs(Frame* frame) {
	uint32_t count = 0;
	void* available_slab = frame->available;
	
	while (available_slab != NULL) {
		count++;
		//available_slab = available_slab->next;
		available_slab = *(void**)available_slab;
	}
	
	return count;
}


// 0s out the memory from the slab to be freed, then adds it to the start 
// of the LL of available slabs
void slab_free(void* location, Frame* frame) {
	if(frame == 0 || location == NULL) { return NULL; }
	
	// zeroing out the old memory COULD be optional.
	// less safe of course, but saves time (though memset is pretty fast)
	//memset(location, 0, frame->slab_size);

	*(void**)location = frame->available;
	frame->available = location;
}

void frame_free(Frame* frame) {
	if(frame == NULL){ return; }

	free(frame->start);

	frame->start = NULL;
	frame->available = NULL;
	frame->slab_size = 0;
	frame->slab_count = 0;
}
