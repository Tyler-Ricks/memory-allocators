#ifndef SLAB_S_H
#define SLAB_S_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <threads.h>

// This is the (hopefully) safer version of the simple slab allocator. It implements a struct that 
// contains memory allocated from the Frame_s. This struct is what is taken as a parameter for 
// memory related operations, which prevents errors with passing in pointers to memory that is not
// a slab on a Frame_s. It also has some more size checks and stuff for making debugging easier
// This all just comes at the cost of user friendliness, because now you access allocated data
// through a struct instead of directly through a pointer.
//
// I am making this version of the slab allocator thread safe as well. For now, I'm doing this with
// a simple mutex to lock threads out so only one thread may influence a Frame at a time. 
// I think more performant approaches to this are possible, but I'll implement that once I have more 
// experience with multithreading.
//
// for simplicity sake, any time frame->available is accessed, or a frame's values are influenced, 
// a lock should be used so only one thread can access the frame at a time.


typedef struct {
	void* start;					// pointer to the full chunk of memory
	void* available;				// pointer to an available chunk 
	size_t slab_size;				// size of each slab in the frame
	uint32_t slab_count;			// number of slabs in the frame
	mtx_t lock;						// mutex for thread safety
}Frame_s;

typedef struct {
	void* memory;					// where the data is stored
	size_t memory_size;				// how big the data is
}Slab_s;

#define FRAME_S_ERROR (Frame_s) { NULL, 0, 0, NULL };

typedef int SLAB_S_RESULT;
#define SLAB_S_FAILURE 0
#define SLAB_S_SUCCESS 1
#define SLAB_S_INVALID_INPUT 2

//static Slab_s* slab_create(void* memory);
//static Slab_s* slab_list_create(void* memory, size_t slab_size, uint32_t slab_count);

void print_void_ptr(void* a);

SLAB_S_RESULT frame_s_create(const size_t slab_size, const uint32_t slab_count, Frame_s* frame);

SLAB_S_RESULT slab_s_alloc_raw(Slab_s* slab, Frame_s* frame);
SLAB_S_RESULT slab_s_alloc(void* data, Slab_s* slab, Frame_s* frame);

uint32_t count_s_available_slabs(Frame_s* frame);

SLAB_S_RESULT slab_s_free(Slab_s* slab, Frame_s* frame);

void frame_s_free(Frame_s* frame);

#endif