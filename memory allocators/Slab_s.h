#ifndef SLAB_S_H
#define SLAB_S_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

// This is the (hopefully) safer version of the simple slab allocator. It implements a struct that 
// contains memory allocated from the Frame. This struct is what is taken as a parameter for 
// memory related operations, which prevents errors with passing in pointers to memory that is not
// a slab on a Frame. It also has some more size checks and stuff for making debugging easier
// This all just comes at the cost of user friendliness, because now you access allocated data
// through a struct instead of directly through a pointer.


typedef struct {
	void* start;					// pointer to the full chunk of memory
	void* available;				// pointer to an available chunk 
	size_t slab_size;				// size of each slab in the frame
	uint32_t slab_count;			// number of slabs in the frame
}Frame;

#define FRAME_ERROR (Frame) { NULL, 0, 0, NULL };

typedef int SLAB_RESULT;
#define SLAB_FAILURE 0
#define SLAB_SUCCESS 1
#define SLAB_INVALID_INPUT 2

//static Slab* slab_create(void* memory);
//static Slab* slab_list_create(void* memory, size_t slab_size, uint32_t slab_count);

SLAB_RESULT frame_create(const size_t slab_size, const uint32_t slab_count, Frame* frame);

void* slab_alloc_raw(Frame* frame);
void* slab_alloc(void* data, Frame* frame);

uint32_t count_available_slabs(Frame* frame);

void slab_free(void* location, Frame* frame);

void frame_free(Frame* frame);

#endif