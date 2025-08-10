#include "Slab_s.h"

//	typedef struct {
//		void* start;					// pointer to the full chunk of memory
//		void* available;				// pointer to an available chunk 
//		size_t slab_size;				// size of each slab in the frame
//		uint32_t slab_count;			// number of slabs in the frame
//		mtx_t lock;						// mutex for thread safety
//	}Frame_s;
//
//	typedef struct {
//		void* memory;					// where data is stored
//		size_t memory_size;				// how large the data is
//	}Slab_s;


void print_void_ptr(void* a) {
	printf("%p\n", a);
}


SLAB_S_RESULT frame_s_create(size_t slab_size, const uint32_t slab_count, Frame_s* frame) {

	// so at the moment, this will not work for storing types that are smaller than 
	// a pointer (such as a float), since a pointer to the next available slab is stored IN an 
	// available slab. I could either just have slabs have a minimum size equal to pointer size,
	// or maybe store an int offset in each available slab
	// for now just have a default slab size be equal to the size of a pointer
	if (slab_size == 0|| slab_count == 0 || frame == NULL) {
		return SLAB_S_INVALID_INPUT;
	}

	if (slab_size < sizeof(void*)) {
		slab_size = sizeof(void*);
	}

	void* chunk = malloc(slab_size * slab_count);
	if(chunk == NULL){ return SLAB_S_FAILURE; }

	// set data in each slab to contain a pointer to the next available slab location
	void* head = chunk;
	for (uint32_t i = 0; i < slab_count - 1; ++i) {
		void* next = (char*)head + slab_size;
		*(void**)head = next;
		head = next;
	}
	*(void**)head = NULL;

	mtx_t lock;
	mtx_init(&lock, mtx_plain);

	frame->start = chunk;
	frame->available = chunk;
	frame->slab_size = slab_size;
	frame->slab_count = slab_count;
	frame->lock = lock;

	return SLAB_S_SUCCESS;
}

// A slab struct should have memory_size filled in by the user before submitting it here.

// Slab_s s1;
// s1.memory_size = sizeof(double);
// if(slab_alloc_raw(&slab, &frame) != SLAB_S_SUCCESS){
//     exit(1);
// }
// double* S1 = (double*)s1.memory;
// *S1 = 5.0;
SLAB_S_RESULT slab_s_alloc_raw(Slab_s* slab, Frame_s* frame) {
	if(slab == NULL || frame == NULL || slab->memory_size > frame->slab_size) { return SLAB_S_INVALID_INPUT; }

	mtx_lock(&frame->lock);

	if (frame->available == NULL) { // NULL when no slabs are available
		mtx_unlock(&frame->lock);
		return SLAB_S_FAILURE;
	}

	slab->memory = frame->available;
	frame->available = *(void**)frame->available;
	

	mtx_unlock(&frame->lock);
	return SLAB_S_SUCCESS;
}


SLAB_S_RESULT slab_s_alloc(void* data, Slab_s* slab, Frame_s* frame) {
	if(slab == NULL || frame == NULL || slab->memory_size > frame->slab_size) { return SLAB_S_INVALID_INPUT; }

	mtx_lock(&frame->lock);

	if (frame->available == NULL) { // NULL when no slabs are available
		mtx_unlock(&frame->lock);
		return SLAB_S_FAILURE;
	}



	slab->memory = frame->available;
	frame->available = *(void**)frame->available;

	memcpy(slab->memory, data, frame->slab_size);

	mtx_unlock(&frame->lock);
	return SLAB_S_SUCCESS;
}


uint32_t count_s_available_slabs(Frame_s* frame) {
	if (frame == NULL) { return 0; }

	mtx_lock(&frame->lock);

	uint32_t count = 0;
	void* available_slab = frame->available;
	
	while (available_slab != NULL) {
		count++;
		//available_slab = available_slab->next;
		available_slab = *(void**)available_slab;
	}
	
	mtx_unlock(&frame->lock);
	return count;
}


// 0s out the memory from the slab to be freed, then adds it to the start 
// of the LL of available slabs
SLAB_S_RESULT slab_s_free(Slab_s* slab, Frame_s* frame) {
	if(frame == NULL || slab == NULL || slab->memory == NULL) { return SLAB_S_INVALID_INPUT; }

	mtx_lock(&frame->lock);
	
	// zeroing out the old memory is probably optional and slower, but safer, so
	memset(slab->memory, 0, frame->slab_size);

	*(void**)slab->memory = frame->available;
	frame->available = slab->memory;

	slab->memory = NULL;
	slab->memory_size = 0;

	mtx_unlock(&frame->lock);
	return SLAB_S_SUCCESS;
}

void frame_s_free(Frame_s* frame) {
	if(frame == NULL){ return; }
	mtx_lock(&frame->lock); // a frame should not be touched after frame_s_free is called

	free(frame->start);

	frame->start = NULL;
	frame->available = NULL;
	frame->slab_size = 0;
	frame->slab_count = 0;

	mtx_unlock(&frame->lock);
	mtx_destroy(&frame->lock);
}
